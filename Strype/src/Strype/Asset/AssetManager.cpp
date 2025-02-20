#include "stypch.h"
#include "AssetManager.h"

#include "Strype/Project/Project.h"
#include "AssetImporter.h"

#include <yaml-cpp/yaml.h>

namespace Strype {

	AssetManager::AssetManager()
	{
		ReloadAssets();
	}

	AssetManager::~AssetManager()
	{
	}

	Ref<Asset> AssetManager::GetAsset(AssetHandle handle)
	{
		if (!IsAssetHandleValid(handle))
			return nullptr;

		Ref<Asset> asset;
		if (IsAssetLoaded(handle))
		{
			asset = m_LoadedAssets.at(handle);
		}
		else
		{
			const AssetMetadata& metadata = GetMetadata(handle);
			asset = AssetImporter::ImportAsset(handle, metadata);
			if (!asset)
			{
				// import failed
				STY_CORE_ERROR("Asset import failed!");
			}
			m_LoadedAssets[handle] = asset;
		}

		return asset;
	}

	bool AssetManager::IsAssetHandleValid(AssetHandle handle) const
	{
		return handle != 0 && m_AssetRegistry.find(handle) != m_AssetRegistry.end();
	}

	bool AssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
	}

	AssetType AssetManager::GetAssetType(AssetHandle handle) const
	{
		if (!IsAssetHandleValid(handle))
			return AssetType::None;

		return m_AssetRegistry.at(handle).Type;
	}

	void AssetManager::ImportAsset(const std::filesystem::path& filepath)
	{
		AssetHandle handle;
		AssetMetadata metadata;
		metadata.Handle = handle;
		metadata.FilePath = std::filesystem::relative(filepath, Project::GetProjectDirectory());
		metadata.Type = Utils::GetAssetTypeFromFileExtension(filepath.extension());

		STY_CORE_ASSERT(metadata.Type != AssetType::None, "Could not import Asset");

		Ref<Asset> asset = AssetImporter::ImportAsset(handle, metadata);

		if (asset)
		{
			asset->Handle = handle;
			m_LoadedAssets[handle] = asset;
			m_LoadedFiles[filepath] = handle;
			m_AssetRegistry[handle] = metadata;
		}
	}

	const AssetMetadata& AssetManager::GetMetadata(AssetHandle handle) const
	{
		static AssetMetadata s_NullMetadata;
		auto it = m_AssetRegistry.find(handle);
		if (it == m_AssetRegistry.end())
			return s_NullMetadata;

		return it->second;
	}

	const std::filesystem::path& AssetManager::GetFilePath(AssetHandle handle) const
	{
		return GetMetadata(handle).FilePath;
	}

	const AssetHandle AssetManager::GetHandle(const std::filesystem::path& path) const
	{
		static AssetHandle s_NullHandle = 0;
		auto it = m_LoadedFiles.find(path);
		if (it == m_LoadedFiles.end())
			return s_NullHandle;

		return it->second;
	}

	void AssetManager::ReloadAssets()
	{
		ProcessDirectory(Project::GetProjectDirectory());
	}

	void AssetManager::ProcessDirectory(const std::filesystem::path& path)
	{
		for (auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory())
				ProcessDirectory(entry.path());
			else if (s_AssetExtensionMap.find(entry.path().extension()) != s_AssetExtensionMap.end())
				ImportAsset(entry.path());
		}
	}

}


