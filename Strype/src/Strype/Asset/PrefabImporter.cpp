#include "stypch.h"
#include "PrefabImporter.h"

#include "Strype/Project/Project.h"
#include "Strype/Room/PrefabSerializer.h"

namespace Strype {

	Ref<Prefab> PrefabImporter::ImportPrefab(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadPrefab(Project::GetProjectDirectory() / metadata.FilePath);
	}

	Ref<Prefab> PrefabImporter::LoadPrefab(const std::filesystem::path& path)
	{
		Ref<Prefab> prefab = CreateRef<Prefab>();
		PrefabSerializer serializer(prefab);
		serializer.Deserialize(path);

		return prefab;
	}

	void PrefabImporter::SavePrefab(Ref<Prefab> prefab, const std::filesystem::path& path)
	{
		PrefabSerializer serializer(prefab);
		serializer.Serialize(Project::GetProjectDirectory() / path);
	}

}

