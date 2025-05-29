#include "stypch.h"
#include "Font.h"

namespace Strype {

	Font::Font(FT_Face face)
	{
        m_Face = face;

        for (uint8_t i = 0; i < 128; i++)
        {
            if (FT_Load_Char(m_Face, i, FT_LOAD_RENDER) != 0)
                continue;

            FT_Bitmap* bitmap = &m_Face->glyph->bitmap;
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
            character.Bearing = glm::ivec2(m_Face->glyph->bitmap_left, m_Face->glyph->bitmap_top);
            character.Advance = m_Face->glyph->advance.x;

            m_Characters[i] = character;
            free(flippedBuffer);
        }
	}

}