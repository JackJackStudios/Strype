#pragma once

#include "Asset.h"
#include "AssetSerializer.h"

namespace Strype {

	static std::map<std::filesystem::path, AssetType> s_AssetExtensionMap = {
		{ ".sprefab", AssetType::Prefab },
		{ ".sroom", AssetType::Room },
		{ ".wav", AssetType::AudioFile },
		{ ".png", AssetType::Sprite },
		{ ".jpg", AssetType::Sprite },
		{ ".jpeg", AssetType::Sprite }
	};

	namespace Utils {

		static AssetType GetAssetTypeFromFileExtension(const std::filesystem::path& extension)
		{
			if (s_AssetExtensionMap.find(extension) == s_AssetExtensionMap.end())
			{
				STY_CORE_WARN("Could not find AssetType for \"{}\" ", extension.string());
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
		bool IsAssetLoaded(const std::filesystem::path& filepath) const;

		AssetHandle ImportAsset(const std::filesystem::path& filepath);
		Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);

		void ReloadAssets();
		void SaveAllAssets();

		const AssetMetadata& GetMetadata(AssetHandle handle) const;
		const std::filesystem::path& GetFilePath(AssetHandle handle) const;
		const AssetType GetAssetType(AssetHandle handle) const;

		AssetHandle GetHandle(const std::filesystem::path& path) const;

		void SaveAsset(AssetHandle handle, const std::filesystem::path& path);
	private:
		void LoadDirectory(const std::filesystem::path& path);

		AssetFileSystem m_LoadedFiles;
		AssetRegistry m_AssetRegistry;
		AssetMap m_LoadedAssets;

		std::unordered_map<AssetType, Scope<AssetSerializer>> m_Serializers;
	};

}