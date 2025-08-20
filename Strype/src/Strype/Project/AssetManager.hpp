#pragma once

#include "stypch.hpp"

#include "Asset.hpp"
#include "AssetSerializer.hpp"

#include <magic_enum/magic_enum.hpp>

namespace Strype {

	static std::map<std::filesystem::path, AssetType> s_AssetExtensionMap = {
		{ ".png", AssetType::Sprite },
		{ ".wav", AssetType::AudioFile },
		{ ".sobject", AssetType::Object },
		{ ".sroom", AssetType::Room },
		{ ".cs", AssetType::Script },
	};

	namespace Utils {

		static AssetType GetAssetTypeFromFileExtension(const std::filesystem::path& extension)
		{
			auto it = s_AssetExtensionMap.find(extension);
			if (it == s_AssetExtensionMap.end())
			{
				STY_CORE_WARN("Could not find AssetType for \"{}\" ", extension);
				return AssetType::None;
			}

			return it->second;
		}

		static bool IsFileInsideFolder(const std::filesystem::path& file, const std::filesystem::path& folder)
		{
			try
			{
				auto absFolder = std::filesystem::weakly_canonical(folder);
				auto absFile = std::filesystem::weakly_canonical(file);

				// Check that absFile starts with absFolder
				return std::mismatch(absFolder.begin(), absFolder.end(), absFile.begin()).first == absFolder.end();
			}
			catch (const std::filesystem::filesystem_error& e)
			{
				return false;
			}
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
	
	using AssetRegistry = std::unordered_map<std::string, AssetMetadata>;
	using AssetMap = std::unordered_map<AssetHandle, Ref<Asset>>;

	using ForEachFunc = std::function<void(AssetHandle)>;
	
	class AssetManager
	{
	public:
		AssetManager();
		~AssetManager();

		Ref<Asset>& GetAsset(AssetHandle handle);

		bool IsAssetLoaded(AssetHandle handle) const;
		bool IsAssetLoaded(const std::string& name) const;
		bool IsAssetLoaded(const std::filesystem::path& filepath) const;

		AssetHandle ImportAsset(std::filesystem::path filepath, AssetHandle handle = AssetHandle());
		Ref<Asset> LoadAsset(const std::filesystem::path& filepath);
		void ReloadAsset(AssetHandle handle);

		void ReloadAssets();
		void SaveAllAssets();

		const std::filesystem::path& GetFilePath(AssetHandle handle) const;
		const std::string& GetName(AssetHandle handle) const;
		AssetType GetAssetType(AssetHandle handle) const;

		AssetHandle GetHandle(const std::string& name) const;
		AssetHandle GetHandle(const std::filesystem::path& filepath) const;

		void CreateAsset(const std::filesystem::path& filepath);
		bool CanCreateAsset(AssetType type, bool createAsset, Ref<Asset>& createdAsset) const;

		void SaveAsset(AssetHandle handle, const std::filesystem::path& filepath);
		void RemoveAsset(AssetHandle handle);

		void MoveAsset(AssetHandle handle, const std::filesystem::path& filepath);

		AssetSerializer* GetSerializer(AssetType type);
		void ForEach(ForEachFunc func)
		{
			for (const auto& [handle, asset] : m_LoadedAssets)
			{
				func(handle);
			}
		}
	private:
		void SetSerializers();
	private:
		AssetRegistry m_AssetRegistry;
		AssetMap m_LoadedAssets;

		std::unordered_map<AssetType, Scope<AssetSerializer>> m_Serializers;
	};

}