#pragma once

#include "Strype/Project/Asset.hpp"
#include "Strype/Renderer/Sprite.hpp"

#include <ft2build.h>
#include <AGI/agi.hpp>

#include FT_FREETYPE_H

namespace Strype {

    struct FontGlyph 
    {
        int advanceX;
        int bearingX;
        int bearingY;
        int width;
        int height;
        float u0, u1, v0, v1;
    };

    struct CharsetRange
    {
        uint32_t Begin, End;
    };

    static const CharsetRange LatinCharacters = { 0x0020, 0x00FF };

    class Font : public Asset
	{
	public:
		Font(FT_Face face, const std::vector<CharsetRange>& ranges);
        ~Font();

        static AssetType GetStaticType() { return AssetType::Font; }
        virtual AssetType GetType() const override { return GetStaticType(); }
    private:
        FT_Face m_FontFace;
        int m_PixelHeight = 48;

        AGI::Texture m_AtlasTexture;
        std::unordered_map<char, FontGlyph> m_Glyphs;

        friend class Renderer;
	};

};