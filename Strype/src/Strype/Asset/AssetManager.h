#pragma once

#include "Asset.h"

namespace Strype {

	static std::map<std::filesystem::path, AssetType> s_AssetExtensionMap = {
		{ ".sprefab", AssetType::Prefab },
		{ ".sroom", AssetType::Room },
		{ ".png", AssetType::Texture },
		{ ".jpg", AssetType::Texture },
		{ ".jpeg", AssetType::Texture }
	};

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
	
	using AssetFileSystem = std::map<std::filesystem::path, AssetHandle>;
	using AssetRegistry = std::map<AssetHandle, AssetMetadata>;
	using AssetMap = std::map<AssetHandle, Ref<Asset>>;

	class AssetManager
	{
	public:
		AssetManager();
		~AssetManager() = default;

		Ref<Asset> GetAsset(AssetHandle handle);

		bool IsAssetLoaded(AssetHandle handle) const;
		bool IsAssetLoaded(std::filesystem::path filepath) const;

		AssetHandle ImportAsset(const std::filesystem::path& filepath);
		void ReloadAssets();

		const AssetMetadata& GetMetadata(AssetHandle handle) const;
		const std::filesystem::path& GetFilePath(AssetHandle handle) const;
		const AssetType GetAssetType(AssetHandle handle) const;

		AssetHandle GetHandle(const std::filesystem::path& path) const;
	private:
		void LoadDirectory(const std::filesystem::path& path);

		AssetFileSystem m_LoadedFiles;
		AssetRegistry m_AssetRegistry;
		AssetMap m_LoadedAssets;
	};

}