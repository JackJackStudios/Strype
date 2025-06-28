#pragma once

#include "Strype/Asset/Asset.hpp"

#include <AGI/agi.hpp>

namespace Strype {

	class Sprite : public Asset
	{
	public:
		Sprite(AGI::Texture texture);

		AGI::Texture& GetTexture() { return m_Texture; }

		static AssetType GetStaticType() { return AssetType::Sprite; }
		virtual AssetType GetType() const override { return GetStaticType(); }

		bool operator==(const Sprite& other) const { return m_Texture == other.m_Texture; }
	private:
		AGI::Texture m_Texture;
	};

}