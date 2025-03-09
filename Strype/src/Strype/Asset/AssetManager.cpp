#include "stypch.h"
#include "AssetManager.h"

#include "Strype/Project/Project.h"
#include "AssetImporter.h"

namespace Strype {

	namespace Utils {

		std::filesystem::path ToAssetSysPath(const std::filesystem::path& filepath)
		{
			return filepath.is_absolute() ? std::filesystem::relative(filepath, Project::GetProjectDirectory()) : filepath;
		}

	}

	AssetManager::AssetManager()
	{
	}
	
	void AssetManager::ReloadAssets()
	{
		LoadDirectory(Project::GetProjectDirectory());
	}

	Ref<Asset> AssetManager::GetAsset(AssetHandle handle)
	{
		Ref<Asset> asset;
		if (IsAssetLoaded(handle))
		{
			asset = m_LoadedAssets.at(handle);
		}
		else
		{
			const AssetMetadata& metadata = GetMetadata(handle);
			asset = AssetImporter::ImportAsset(handle, metadata);

			m_LoadedAssets[handle] = asset;
		}

		return asset;
	}

	bool AssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
	}

	bool AssetManager::IsAssetLoaded(const std::filesystem::path& filepath) const
	{
		return m_LoadedFiles.find(Utils::ToAssetSysPath(filepath)) != m_LoadedFiles.end();
	}

	const AssetType AssetManager::GetAssetType(AssetHandle handle) const
	{
		return m_AssetRegistry.at(handle).Type;
	}

	const std::filesystem::path& AssetManager::GetFilePath(AssetHandle handle) const
	{
		return GetMetadata(handle).FilePath;
	}

	AssetHandle AssetManager::GetHandle(const std::filesystem::path& path) const
	{
		return m_LoadedFiles.at(Utils::ToAssetSysPath(path));
	}

	AssetHandle AssetManager::ImportAsset(const std::filesystem::path& filepath)
	{
		AssetHandle handle;
		AssetMetadata metadata;
		metadata.Handle = handle;
		metadata.FilePath = Utils::ToAssetSysPath(filepath);
		metadata.Type = Utils::GetAssetTypeFromFileExtension(filepath.extension());

		STY_CORE_ASSERT(metadata.Type != AssetType::None, "Could not import Asset");

		Ref<Asset> asset = AssetImporter::ImportAsset(handle, metadata);

		if (asset)
		{
			asset->Handle = handle;
			m_LoadedAssets[handle] = asset;
			m_LoadedFiles[metadata.FilePath] = handle;
			m_AssetRegistry[handle] = metadata;
		}
		else
		{
			STY_CORE_ERROR("Asset import failed '{0}'", metadata.FilePath.string());
		}

		return handle;
	}

	const AssetMetadata& AssetManager::GetMetadata(AssetHandle handle) const
	{
		return m_AssetRegistry.at(handle);
	}

	void AssetManager::LoadDirectory(const std::filesystem::path& path)
	{
		for (auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory())
				LoadDirectory(entry.path());
			else if (s_AssetExtensionMap.find(entry.path().extension()) != s_AssetExtensionMap.end())
				ImportAsset(std::filesystem::relative(entry.path(), Project::GetProjectDirectory()));
		}
	}

}


