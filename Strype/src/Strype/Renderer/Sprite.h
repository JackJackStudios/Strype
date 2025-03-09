#pragma once

#include "Strype/Asset/Asset.h"

#include <agi.h>

namespace Strype {

	class Sprite : public Asset
	{
	public:
		Sprite(Ref<AGI::Texture> texture);
		Sprite(const std::filesystem::path& filepath);

		static AssetType GetStaticType() { return AssetType::Texture; }
		virtual AssetType GetType() const override { return GetStaticType(); }
	public:
		Ref<AGI::Texture> Texture;
	};

}