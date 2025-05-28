#pragma once

#include "Strype/Asset/Asset.h"

#include <AGI/agi.hpp>

namespace Strype {

	class Sprite : public Asset
	{
	public:
		Sprite(std::shared_ptr<AGI::Texture> texture);

		Ref<AGI::Texture>& GetTexture() { return m_Texture; }

		static AssetType GetStaticType() { return AssetType::Sprite; }
		virtual AssetType GetType() const override { return GetStaticType(); }

		bool operator==(const Sprite& other) const { return m_Texture == other.m_Texture; }
	private:
		Ref<AGI::Texture> m_Texture;
	};

}