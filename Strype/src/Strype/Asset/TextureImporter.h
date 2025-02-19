#pragma once

#include "Asset.h"

#include "Strype/Renderer/Texture.h"

namespace Strype {

	class TextureImporter
	{
	public:
		static Ref<Texture> ImportTexture(AssetHandle handle, const AssetMetadata& metadata);

		static Ref<Texture> LoadTexture(const std::filesystem::path& path);
	};

}