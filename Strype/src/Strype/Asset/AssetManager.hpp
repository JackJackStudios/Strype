#pragma once

#include "stypch.hpp"

#include "Asset.hpp"
#include "AssetSerializer.hpp"

#include <magic_enum/magic_enum.hpp>

namespace Strype {

	static std::map<std::filesystem::path, AssetType> s_AssetExtensionMap = {
		{ ".png", AssetType::Sprite },
		{ ".wav", AssetType::AudioFile },
		{ ".cs", AssetType::Object },
		{ ".sroom", AssetType::Room },
	};

	namespace Utils {

		static AssetType GetAssetTypeFromFileExtension(const std::filesystem::path& extension)
		{
			if (s_AssetExtensionMap.find(extension) == s_AssetExtensionMap.end())
			{
				STY_CORE_WARN("Could not find AssetType for \"{}\" ", extension);
				return AssetType::None;
			}

			return s_AssetExtensionMap.at(extension);
		}

		const std::filesystem::path& GetFileExtensionFromAssetType(AssetType type);

	}
	
	using AssetFileSystem = std::map<std::filesystem::path, AssetHandle>;
	using AssetRegistry = std::map<AssetHandle, AssetMetadata>;
	
	class AssetManager
	{
	public:
		AssetManager();
		~AssetManager();

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

		void CreateAsset(const std::filesystem::path& path);
		bool CanCreateAsset(AssetType type, bool createAsset, Ref<Asset>& createdAsset);

		void SaveAsset(AssetHandle handle, const std::filesystem::path& path);
		void RemoveAsset(AssetHandle handle);

		AssetSerializer* GetSerializer(AssetType type);
	private:
		void LoadDirectory(const std::filesystem::path& path);

		AssetFileSystem m_LoadedFiles;
		AssetRegistry m_AssetRegistry;

		std::unordered_map<AssetType, Scope<AssetSerializer>> m_Serializers;
	};

}