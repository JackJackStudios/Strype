#pragma once

#include "Strype/Asset/AssetSerializer.h"

#include "Prefab.h"

namespace Strype {

	class PrefabSerializer: public AssetSerializer
	{
	public:
		virtual Ref<Asset> LoadAsset(const std::filesystem::path& path);
		
		virtual void SaveAsset(Ref<Asset> asset, const std::filesystem::path& path);
	};

}