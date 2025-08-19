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

    void AssetManager::SetSerializers()
    {
        m_Serializers.clear();
        m_Serializers[AssetType::Object] = CreateScope<ObjectSerializer>();
        m_Serializers[AssetType::Sprite] = CreateScope<SpriteSerializer>();
        m_Serializers[AssetType::Room] = CreateScope<RoomSerializer>();
        m_Serializers[AssetType::AudioFile] = CreateScope<AudioFileSerializer>();
        m_Serializers[AssetType::Script] = CreateScope<ScriptSerializer>();
    }

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
        
        std::regex pattern(".strip(\\d+)");
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

    Ref<Asset> ScriptSerializer::LoadAsset(const std::filesystem::path& path)
    {
        auto& scriptEngine = Project::GetScriptEngine();
        ScriptID script = scriptEngine->GetIDByName(path.stem().string());
        if (!scriptEngine->IsValidScript(script))
        {
            STY_CORE_WARN("{} doesn't have corrosponding class in C# binaries", path.filename());
            return nullptr;
        }

        return CreateRef<Script>(script);
    }

    // Save template for newly created scripts
    namespace Utils {

        static void ReplaceKeyWord(std::string& str, const std::string& keyword, const std::string& replace)
        {
            size_t pos = 0;
            while ((pos = str.find(keyword, pos)) != std::string::npos)
            {
                str.replace(pos, keyword.length(), replace);
                pos += replace.length();
            }
        }
    };

    void ScriptSerializer::SaveAsset(Ref<Asset> asset, const std::filesystem::path& path)
    {
        Script* script = (Script*)asset.get();
        if (script->GetID() != 0) return;

        std::string scriptTemplate = Utils::ReadFile("assets/ScriptTemplate.cs");

        Utils::ReplaceKeyWord(scriptTemplate, "Template", path.stem().string());
        Utils::WriteFile(path, scriptTemplate);
    }

}

