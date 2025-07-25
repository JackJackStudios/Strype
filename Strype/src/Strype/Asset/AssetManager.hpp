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

		static const std::filesystem::path& GetFileExtensionFromAssetType(AssetType type)
		{
			for (const auto& [extension, assetType] : s_AssetExtensionMap)
			{
				if (assetType == type)
					return extension;
			}

			STY_CORE_WARN("Could not find file extension for \"{}\" ", magic_enum::enum_name(type));
			return "";
		}

	}
	
	using AssetRegistry = std::unordered_map<std::filesystem::path, AssetHandle>;
	using AssetMap = std::unordered_map<AssetHandle, Ref<Asset>>;

	using ForEachFunc = std::function<void(AssetHandle)>;
	
	class AssetManager
	{
	public:
		AssetManager();
		~AssetManager();

		Ref<Asset> GetAsset(AssetHandle handle);

		bool IsAssetLoaded(AssetHandle handle) const;
		bool IsAssetLoaded(const std::filesystem::path& filepath) const;

		AssetHandle ImportAsset(std::filesystem::path filepath, AssetHandle handle = AssetHandle());
		Ref<Asset> LoadAsset(const std::filesystem::path& filepath);

		void ReloadAssets();
		void SaveAllAssets();

		const std::filesystem::path& GetFilePath(AssetHandle handle);
		AssetType GetAssetType(AssetHandle handle) const;

		AssetHandle GetHandle(const std::filesystem::path& filepath) const;

		void CreateAsset(const std::filesystem::path& filepath);
		bool CanCreateAsset(AssetType type, bool createAsset, Ref<Asset>& createdAsset) const;

		void SaveAsset(AssetHandle handle, const std::filesystem::path& filepath);
		void RemoveAsset(AssetHandle handle);

		void MoveAsset(AssetHandle handle, const std::filesystem::path& filepath);

		AssetSerializer* GetSerializer(AssetType type);
		void ForEach(ForEachFunc func)
		{
			for (const auto& [handle, asset] : m_AssetRegistry)
			{
				func(handle);
			}
		}
	private:
		AssetRegistry m_LoadedFiles;
		AssetMap m_AssetRegistry;

		std::unordered_map<AssetType, Scope<AssetSerializer>> m_Serializers;
	};

}