#include "stypch.hpp"
#include "Font.hpp"

#include "Renderer.hpp"

namespace Strype {

    class AtlasPacker
    {
    public:
        AtlasPacker(const glm::ivec2& size)
        {
            m_Size = size;
            m_Pixels.resize(size.x * size.y);
        }

        glm::ivec2 PackAndBlit(const uint8_t* src, int srcWidth, int srcHeight, int srcPitch, int padding = 1)
        {
            if (srcWidth == 0 || srcHeight == 0)
                return { 0, 0 };

            // Check if we need to wrap to next row
            if (m_CurrentPos.x + srcWidth + padding > m_Size.x)
            {
                m_CurrentPos.x = 0;
                m_CurrentPos.y += m_RowHeight + padding;
                m_RowHeight = 0;
            }

            EnsureHeight(m_CurrentPos.y + srcHeight + padding);
            if (m_CurrentPos.y + srcHeight + padding > m_Size.y)
            {
                STY_CORE_ERROR("Atlas overflow even after resize. Increase initial size.");
                return { 0, 0 };
            }

            int dstX = m_CurrentPos.x;
            int dstY = m_CurrentPos.y;

            // Copy each row of pixels
            for (int y = 0; y < srcHeight; ++y)
            {
                const uint8_t* srcRow = src + y * srcPitch;
                uint8_t* dstRow = m_Pixels.data() + ((m_CurrentPos.y + y) * m_Size.x + m_CurrentPos.x);
                std::memcpy(dstRow, srcRow, static_cast<size_t>(srcWidth));
            }

            // Advance position for next sprite
            m_CurrentPos.x += srcWidth + padding;
            if (srcHeight + padding > m_RowHeight)
                m_RowHeight = srcHeight + padding;

            return { dstX, dstY };
        }

        const std::vector<uint8_t>& GetPixels() const { return m_Pixels; }
        const glm::ivec2& GetSize() const { return m_Size; }
    private:
        void EnsureHeight(int neededY)
        {
            if (neededY <= m_Size.y) return;

            int newHeight = m_Size.y;
            while (newHeight < neededY)
                newHeight *= 2;

            m_Pixels.resize(m_Size.x * newHeight);
            m_Size.y = newHeight;
        }

    private:
        glm::ivec2 m_Size = { 0, 0 };
        glm::ivec2 m_CurrentPos = { 0, 0 };
        int m_RowHeight = 0;

        std::vector<uint8_t> m_Pixels;
    };

	Font::Font(FT_Face face, const std::vector<CharsetRange>& ranges)
        : m_FontFace(face)
	{
        AtlasPacker atlas({ 1024, 1024 });

        for (const auto& charset : ranges)
        {
            for (uint32_t c = charset.Begin; c <= charset.End; ++c)
            {
                if (FT_Load_Char(m_FontFace, c, FT_LOAD_RENDER) != 0)
                    continue;

                FT_GlyphSlot g = m_FontFace->glyph;
                FT_Bitmap& bm = g->bitmap;

                int src_w = (int)bm.width;
                int src_h = (int)bm.rows;
                const uint8_t* src_buf = bm.buffer;
                int src_pitch = bm.pitch > 0 ? bm.pitch : src_w;

                glm::ivec2 pos = atlas.PackAndBlit(src_buf, src_w, src_h, src_pitch);

                FontGlyph glyphData;
                glyphData.advanceX = (g->advance.x >> 6);
                glyphData.bearingX = g->bitmap_left;
                glyphData.bearingY = g->bitmap_top;
                glyphData.width = src_w;
                glyphData.height = src_h;

                // UVs in normalized 0..1
                glyphData.u0 = float(pos.x) / float(atlas.GetSize().x);
                glyphData.v0 = float(pos.y) / float(atlas.GetSize().y);
                glyphData.u1 = float(pos.x + src_w) / float(atlas.GetSize().x);
                glyphData.v1 = float(pos.y + src_h) / float(atlas.GetSize().y);

                m_Glyphs[(char)c] = glyphData;
            }
        }

        AGI::TextureSpecification specification;
        specification.Size = atlas.GetSize();
        specification.Format = AGI::ImageFormat::RED;

        specification.Data = (void*)atlas.GetPixels().data();
        specification.Datasize = atlas.GetPixels().size() * sizeof(uint8_t);
        m_AtlasTexture = Renderer::GetCurrent()->GetContext()->CreateTexture(specification);

        ExampleText("Hello, FreeType!");
	}

    Font::~Font()
    {
        FT_Done_Face(m_FontFace);
    }

    void Font::ExampleText(const std::string& text)
    {
        int pen_x = 0;
        int pen_y = m_PixelHeight; // baseline roughly at pixel_height (depends on font)
        std::cout << "Sample layout for: \"" << text << "\"\n";
        for (char ch : text) 
        {
            auto it = m_Glyphs.find(ch);
            if (it == m_Glyphs.end()) { pen_x += m_PixelHeight / 2; continue; } // fallback gap
            const FontGlyph& G = it->second;

            // Position where the glyph bitmap should be drawn (top-left)
            int glyph_x = pen_x + G.bearingX;
            int glyph_y = pen_y - G.bearingY;

            // UVs are (G.u0,G.v0)-(G.u1,G.v1)
            std::cout << "'" << ch << "': advance=" << G.advanceX
                << " pos=(" << glyph_x << "," << glyph_y << ") size=("
                << G.width << "x" << G.height << ") uv=("
                << G.u0 << "," << G.v0 << ")-(" << G.u1 << "," << G.v1 << ")\n";

            pen_x += G.advanceX;
        }
    }

};