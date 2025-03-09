#pragma once

#include "Strype/Renderer/Sprite.h"

#include "Asset.h"

namespace Strype {

	class TextureImporter
	{
	public:
		static Ref<Sprite> ImportTexture(AssetHandle handle, const AssetMetadata& metadata);
		
		static Ref<Sprite> LoadTexture(const std::filesystem::path& path);
	};

}