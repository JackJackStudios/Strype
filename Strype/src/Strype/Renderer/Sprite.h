#pragma once

#include "Strype/Asset/Asset.h"

#include <AGI/agi.h>

namespace Strype {

	class Sprite : public Asset
	{
	public:
		Sprite(std::shared_ptr<AGI::Texture> texture);

		static AssetType GetStaticType() { return AssetType::Sprite; }
		virtual AssetType GetType() const override { return GetStaticType(); }
	public:
		Ref<AGI::Texture> Texture;
	};

}