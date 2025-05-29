#pragma once

#include <AGI/agi.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Strype/Asset/Asset.h"

namespace Strype {

	struct Character 
	{
		std::shared_ptr<AGI::Texture> Texture;
		glm::ivec2 Size;     
		glm::ivec2 Bearing;  
		uint32_t Advance;  
	};


	class Font : public Asset
	{
	public:
		Font(FT_Face face);

		const Character& GetCharacter(char character) { return m_Characters.at(character); }

		static AssetType GetStaticType() { return AssetType::Room; }
		virtual AssetType GetType() const override { return GetStaticType(); }
	private:
		std::unordered_map<char, Character> m_Characters;
		FT_Face m_Face;

		friend class FontSerializer;
	};

}