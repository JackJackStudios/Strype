#pragma once

#include "Asset.hpp"

#include <magic_enum/magic_enum.hpp>

namespace Strype {
	
	using AssetRegistry = std::unordered_map<std::string, AssetMetadata>;
	using AssetMap = std::unordered_map<AssetHandle, Ref<Asset>>;

	class Project;

	class AssetManager
	{
	public:
		void LoadAllAssets(Ref<Project> proj = nullptr);
		void SaveAllAssets();

		bool IsAssetLoaded(AssetHandle handle) const;
		bool IsAssetFile(AssetHandle handle) const;

		AssetHandle ImportAsset(const std::filesystem::path& filepath);

		void SaveAsset(AssetHandle handle, const std::filesystem::path& filepath = std::filesystem::path());
		Ref<Asset> LoadAsset(const std::filesystem::path& filepath);

		Ref<Asset> GetAsset(AssetHandle handle);
		const Ref<Asset>& GetAsset(AssetHandle handle) const;
		
		const std::filesystem::path& GetFilePath(AssetHandle handle) const;
		const std::string& GetName(AssetHandle handle) const;
		AssetType GetAssetType(AssetHandle handle) const;
		AssetHandle GetHandle(const std::string& name) const;
		
		void RemoveAsset(AssetHandle handle);
		void ReloadAsset(AssetHandle handle);
	private:
		AssetRegistry m_AssetRegistry;
		AssetMap m_LoadedAssets;

		Ref<Project> m_LoadedProject;
	};

}