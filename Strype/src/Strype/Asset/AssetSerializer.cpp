#include "stypch.hpp"
#include "AssetSerializer.hpp"

#include "Strype/Script/ScriptEngine.hpp"
#include "Strype/Project/Project.hpp"
#include "Strype/Renderer/Sprite.hpp"
#include "Strype/Utils/YamlHelpers.hpp"
#include "Strype/Renderer/Renderer.hpp"
#include "Strype/Audio/Audio.hpp"

#include <AGI/agi.hpp>
#include <sndfile.h>
#include <stb_image.h>

#include <regex>

namespace Strype {

    Ref<Asset> SpriteSerializer::LoadAsset(const std::filesystem::path& path)
    {
        int frameCount = 1;
        int width, height, channels;

        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
        STY_VERIFY(data, "Failed to load sprite \"{}\" ", path);

        AGI::TextureSpecification textureSpec;
        textureSpec.Width = width;
        textureSpec.Height = height;
        textureSpec.Format = AGI::Utils::ChannelsToImageFormat(channels);
        AGI::Texture texture = Renderer::GetContext()->CreateTexture(textureSpec);

        texture->SetData(data, width * height * channels);
        stbi_image_free(data);

        std::regex pattern("_strip(\\d+)");
        std::smatch match;

        std::string pathStr = path.string();
        if (std::regex_search(pathStr, match, pattern))
            frameCount = std::stoi(match[1].str());

        return CreateRef<Sprite>(texture, frameCount);
    }

    Ref<Asset> AudioFileSerializer::LoadAsset(const std::filesystem::path& path)
    {
        SF_INFO sfinfo;
        SNDFILE* sndfile = sf_open(path.string().c_str(), SFM_READ, &sfinfo);
        STY_CORE_VERIFY(sndfile, "Could not open sound file");

		Buffer membuf;
		membuf.Allocate(sfinfo.frames * sfinfo.channels * sizeof(short));

        sf_count_t num_frames = sf_readf_short(sndfile, (short*)membuf.Data, sfinfo.frames);
        STY_CORE_VERIFY(num_frames == sfinfo.frames, "Error reading audio file data");

        Ref<AudioFile> file = CreateRef<AudioFile>(sfinfo.frames, sfinfo.channels, sfinfo.samplerate);
        file->SetData(membuf);
		
        sf_close(sndfile);
		membuf.Release();

        return file;
    }

	Ref<Asset> ObjectSerializer::LoadAsset(const std::filesystem::path& path)
	{
		Ref<Object> object = CreateRef<Object>();

		auto& scriptEngine = Project::GetScriptEngine();
        ScriptID script = scriptEngine->GetIDByName(path.stem().string());
        const ScriptField& textureField = scriptEngine->GetField(script, "TexturePath");

        std::string coralName = std::string(*textureField.DefaultValue.As<Coral::String>());

		object->ClassID = script;
        object->TextureHandle = Project::ImportAsset(coralName);

		return object;
	}

}

