#include "stypch.hpp"
#include "AssetSerializer.hpp"

#include "Strype/Script/ScriptEngine.hpp"
#include "Strype/Project/Project.hpp"
#include "Strype/Renderer/Sprite.hpp"
#include "Strype/Utils/YamlHelpers.hpp"
#include "Strype/Renderer/Renderer.hpp"
#include "Strype/Audio/Audio.hpp"

#include <AGI/agi.hpp>
#include <stb_image.h>

#include <regex>

namespace Strype {

    Ref<Asset> SpriteSerializer::LoadAsset(const std::filesystem::path& path)
    {
        int frameCount = 1;
        int width, height, channels;

        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
        if (!data) return nullptr;

        AGI::TextureSpecification textureSpec;
        textureSpec.Width = width;
        textureSpec.Height = height;
        textureSpec.Format = AGI::Utils::ChannelsToImageFormat(channels);

        textureSpec.Data = data;
        textureSpec.Datasize = width * height * channels;
        
        std::regex pattern("_strip(\\d+)");
        std::smatch match;

        std::string pathStr = path.string();
        if (std::regex_search(pathStr, match, pattern))
            frameCount = std::stoi(match[1].str());

        return CreateRef<Sprite>(textureSpec, frameCount);
    }

    Ref<Asset> AudioFileSerializer::LoadAsset(const std::filesystem::path& path)
    {
        ma_decoder decoder;
        ma_result result = ma_decoder_init_file(path.string().c_str(), nullptr, &decoder);
        if (result != MA_SUCCESS) return nullptr;

        return CreateRef<AudioFile>(decoder);
    }

	Ref<Asset> ObjectSerializer::LoadAsset(const std::filesystem::path& path)
	{
        YAML::Node root = YAML::LoadFile(path.string())["Object"];
        if (!root) return nullptr;

        auto& scriptEngine = Project::GetScriptEngine();
        ScriptID script = scriptEngine->GetIDByName(root["Script"].as<std::string>());
        if (!scriptEngine->IsValidScript(script))
            return nullptr;

		Ref<Object> object = CreateRef<Object>();
        object->TextureHandle = Project::ImportAsset(root["SpritePath"].as<std::filesystem::path>());
        object->ClassID = script;

		return object;
	}

}

