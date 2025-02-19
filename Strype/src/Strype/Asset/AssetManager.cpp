#include "stypch.h"
#include "AssetManager.h"

#include "Strype/Project/Project.h"
#include "AssetImporter.h"

#include <yaml-cpp/yaml.h>

namespace Strype {
	
	namespace Utils {

		static AssetType GetAssetTypeFromFileExtension(const std::filesystem::path& extension)
		{
			if (s_AssetExtensionMap.find(extension) == s_AssetExtensionMap.end())
			{
				STY_CORE_WARN("Could not find AssetType for {0}", extension.string());
				return AssetType::None;
			}

			return s_AssetExtensionMap.at(extension);
		}

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
		metadata.FilePath = filepath;
		metadata.Type = Utils::GetAssetTypeFromFileExtension(filepath.extension());

		STY_CORE_ASSERT(metadata.Type != AssetType::None, "Could not import Asset");

		Ref<Asset> asset = AssetImporter::ImportAsset(handle, metadata);

		if (asset)
		{
			asset->Handle = handle;
			m_LoadedAssets[handle] = asset;
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

}


