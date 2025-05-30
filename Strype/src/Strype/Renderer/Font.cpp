#include "stypch.h"
#include "Font.h"

namespace Strype {

    Font::Font(FT_Face face)
    {
        m_Face = face;

        unsigned char* atlasBuffer = (unsigned char*)calloc(ATLAS_WIDTH * ATLAS_HEIGHT, 1);

        uint32_t maxWidth = 0;
        uint32_t maxHeight = 0;
        
        uint32_t penX = 0;
        uint32_t penY = 0;
        uint32_t rowHeight = 0;

        for (uint8_t i = 0; i < 128; i++)
        {
            if (FT_Load_Char(m_Face, i, FT_LOAD_RENDER) != 0)
                continue;

            FT_Bitmap* bitmap = &m_Face->glyph->bitmap;
            maxWidth = glm::max(maxWidth, bitmap->width);
            maxHeight = glm::max(maxHeight, bitmap->rows);
        }

        for (uint8_t i = 0; i < 128; i++) 
        {
            if (FT_Load_Char(m_Face, i, FT_LOAD_RENDER) != 0)
                continue;

            FT_Bitmap* bitmap = &m_Face->glyph->bitmap;
            if (penX + bitmap->width >= ATLAS_WIDTH)
            {
                penX = 0;
                penY += rowHeight;
                rowHeight = 0;
            }

            Character character;
            character.UVOffset = glm::vec2((float)penX / ATLAS_WIDTH, (float)penY / ATLAS_HEIGHT);
            character.UVSize = glm::vec2((float)bitmap->width / ATLAS_WIDTH, (float)bitmap->rows / ATLAS_HEIGHT);
            character.Size = glm::ivec2(bitmap->width, bitmap->rows);
            character.Bearing = glm::ivec2(m_Face->glyph->bitmap_left, m_Face->glyph->bitmap_top);
            character.Advance = m_Face->glyph->advance.x;
            
            // Copy glyph bitmap to atlas
            for (unsigned int y = 0; y < character.Size.y; y++)
            {
                int flippedY = character.Size.y - 1 - y;
                for (unsigned int x = 0; x < character.Size.x; x++)
                {
                    int dstX = penX + x;
                    int dstY = penY + flippedY;
                    atlasBuffer[dstY * ATLAS_WIDTH + dstX] = bitmap->buffer[y * character.Size.x + x];
                }
            }

            m_Characters[i] = character;

            penX += character.Size.x + 1;
            rowHeight = glm::max((int)rowHeight, character.Size.y);
        }

        // Create texture atlas
        AGI::TextureSpecification textureSpec;
        textureSpec.Width = ATLAS_WIDTH;
        textureSpec.Height = ATLAS_HEIGHT;
        textureSpec.Format = AGI::ImageFormat::RED;

        m_Atlas = AGI::Texture::Create(textureSpec);
        m_Atlas->SetData(atlasBuffer, ATLAS_WIDTH * ATLAS_HEIGHT);

        free(atlasBuffer);
    }

}