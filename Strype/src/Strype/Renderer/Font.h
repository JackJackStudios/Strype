#pragma once

#include <AGI/agi.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Strype/Asset/Asset.h"
#include "Strype/Renderer/RenderPipeline.h"

namespace Strype {

	struct Character 
	{
		glm::vec2 UVOffset;
		glm::vec2 UVSize;
		glm::ivec2 Size;     
		glm::ivec2 Bearing;  
		uint32_t Advance;
	};

	class Font : public Asset
	{
	public:
		Font(FT_Face face);

		const Character& GetCharacter(char character) const { return m_Characters.at(character); }
		const glm::vec2& GetMaxSize() const { return m_MaxSize; }

		const std::shared_ptr<AGI::Texture>& GetAtlas() const { return m_Atlas; }

		static AssetType GetStaticType() { return AssetType::Room; }
		virtual AssetType GetType() const override { return GetStaticType(); }
	private:
		static constexpr int ATLAS_WIDTH = 512;
		static constexpr int ATLAS_HEIGHT = 512;
	private:
		std::shared_ptr<AGI::Texture> m_Atlas;
		std::unordered_map<char, Character> m_Characters;
		FT_Face m_Face;

		glm::vec2 m_MaxSize;

		friend class FontSerializer;
	};

}