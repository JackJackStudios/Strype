#include "stypch.hpp"
#include "AssetSerializer.hpp"

#include "Strype/Project/Project.hpp"
#include "Strype/Renderer/Sprite.hpp"
#include "Strype/Renderer/Renderer.hpp"
#include "Strype/Audio/Audio.hpp"

#include <AGI/agi.hpp>
#include <stb_image.h>
#include <sndfile.h>

namespace Strype {

    Ref<Asset> SpriteSerializer::LoadAsset(const std::filesystem::path& path)
    {
        int width, height, channels;

        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

        STY_VERIFY(data, "Failed to load sprite \"{}\" ", path.string());

        AGI::TextureSpecification textureSpec;
        textureSpec.Width = width;
        textureSpec.Height = height;
        textureSpec.Format = AGI::ChannelsToImageFormat(channels);

        std::shared_ptr<AGI::Texture> texture = Renderer::GetContext()->CreateTexture(textureSpec);
        texture->SetData(data, width * height * channels);

        return CreateRef<Sprite>(texture);
    }

    Ref<Asset> AudioFileSerializer::LoadAsset(const std::filesystem::path& path)
    {
        SF_INFO sfinfo;
        SNDFILE* sndfile = sf_open(path.string().c_str(), SFM_READ, &sfinfo);
        
        STY_CORE_VERIFY(sndfile, "Could not open sound file");

        short* membuf = static_cast<short*>(malloc(static_cast<size_t>(sfinfo.frames * sfinfo.channels) * sizeof(short)));
        sf_count_t num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
        uint32_t num_bytes = (num_frames * sfinfo.channels) * sizeof(short);

        STY_CORE_VERIFY(num_frames == sfinfo.frames, "Error reading audio file data");

        Ref<AudioFile> file = CreateRef<AudioFile>(sfinfo.frames, sfinfo.channels, sfinfo.samplerate);
        file->SetData(Buffer(membuf, num_bytes));

        sf_close(sndfile);
        free(membuf);

        return file;
    }

}

