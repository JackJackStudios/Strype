#pragma once

#include "Asset.h"

namespace Strype {

	static std::map<std::filesystem::path, AssetType> s_AssetExtensionMap = {
		{ ".sroom", AssetType::Room },
		{ ".png", AssetType::Texture },
		{ ".jpg", AssetType::Texture },
		{ ".jpeg", AssetType::Texture }
	};

	using AssetRegistry = std::map<AssetHandle, AssetMetadata>;
	using AssetMap = std::map<AssetHandle, Ref<Asset>>;

	class AssetManager
	{
	public:
		Ref<Asset> GetAsset(AssetHandle handle);

		bool IsAssetHandleValid(AssetHandle handle) const;
		bool IsAssetLoaded(AssetHandle handle) const;
		AssetType GetAssetType(AssetHandle handle) const;

		void ImportAsset(const std::filesystem::path& filepath);

		const AssetMetadata& GetMetadata(AssetHandle handle) const;
		const std::filesystem::path& GetFilePath(AssetHandle handle) const;

		const AssetRegistry& GetAssetRegistry() const { return m_AssetRegistry; }
	private:
		AssetRegistry m_AssetRegistry;
		AssetMap m_LoadedAssets;
	};

}