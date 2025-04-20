#pragma once

#include "Strype/Renderer/Sprite.h"

#include "Asset.h"
#include "AssetSerializer.h"

namespace Strype {

	class SpriteSerializer : public AssetSerializer
	{
	public:
		virtual Ref<Asset> LoadAsset(const std::filesystem::path& path)
		{
			return CreateRef<Sprite>(Project::GetProjectDirectory() / path);
		}

		virtual void SaveAsset(Ref<Asset> asset, const std::filesystem::path& path) {};
	};

}