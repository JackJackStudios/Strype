#include "stypch.h"
#include "AssetSerializer.h"

#include "Strype/Project/Project.h"
#include "Strype/Renderer/Sprite.h"
#include "Strype/Renderer/Font.h"
#include "Strype/Renderer/Renderer.h"
#include "Strype/Audio/Audio.h"

#include <AGI/agi.hpp>
#include <stb_image.h>
#include <sndfile.h>
#include <ft2build.h>
#include FT_FREETYPE_H

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

        std::shared_ptr<AGI::Texture> texture = AGI::Texture::Create(textureSpec);
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

    Ref<Asset> FontSerializer::LoadAsset(const std::filesystem::path& path)
    {
        Ref<Font> font = CreateRef<Font>();
        FT_Library ft = Renderer::GetPipeline<TextPipeline>()->m_FreetypeLib;

        FT_New_Face(ft, path.string().c_str(), 0, &font->m_Face);
        FT_Set_Pixel_Sizes(font->m_Face, 0, 48);

        for (uint8_t i = 0; i < 128; i++)
        {
            if (FT_Load_Char(font->m_Face, i, FT_LOAD_RENDER) != 0)
                continue;

            FT_Bitmap* bitmap = &font->m_Face->glyph->bitmap;
            unsigned char* flippedBuffer = (unsigned char*)malloc(bitmap->width * bitmap->rows * sizeof(unsigned char));

            for (unsigned int y = 0; y < bitmap->rows; ++y) {
                memcpy(
                    flippedBuffer + (bitmap->rows - 1 - y) * bitmap->width,
                    bitmap->buffer + y * bitmap->width,
                    bitmap->width
                );
            }

            AGI::TextureSpecification textureSpec;
            textureSpec.Width = bitmap->width;
            textureSpec.Height = bitmap->rows;
            textureSpec.Format = AGI::ImageFormat::RED;

            std::shared_ptr<AGI::Texture> texture = AGI::Texture::Create(textureSpec);
            texture->SetData(flippedBuffer, bitmap->width * bitmap->rows * sizeof(unsigned char));

            Character character;
            character.Texture = texture;
            character.Size = glm::ivec2(bitmap->width, bitmap->rows);
            character.Bearing = glm::ivec2(font->m_Face->glyph->bitmap_left, font->m_Face->glyph->bitmap_top);
            character.Advance = font->m_Face->glyph->advance.x;

            font->m_Characters[i] = character;
            free(flippedBuffer);
        }

        return font;
    }

}

