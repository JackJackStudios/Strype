#pragma once

#include "Asset.h"

#include "Strype/Room/Prefab.h"

namespace Strype {

	class PrefabImporter
	{
	public:
		static Ref<Prefab> ImportPrefab(AssetHandle handle, const AssetMetadata& metadata);
		
		static Ref<Prefab> LoadPrefab(const std::filesystem::path& path);

		static void SavePrefab(Ref<Prefab> prefab, const std::filesystem::path& path);
	};

}