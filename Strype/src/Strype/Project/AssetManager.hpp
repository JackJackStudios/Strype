#pragma once

#include "Asset.hpp"

#include <magic_enum/magic_enum.hpp>

namespace Strype {
	
	static std::unordered_map<std::filesystem::path, AssetType> s_AssetExtensionMap = {
		{ ".png",     AssetType::Sprite },
		{ ".jpg",     AssetType::Sprite },
		{ ".jpeg",    AssetType::Sprite },
		{ ".bmp",     AssetType::Sprite },
					  
		{ ".wav",     AssetType::AudioFile },
#ifdef MA_HAS_MP3
		{ ".mp3",     AssetType::AudioFile },
#endif
#ifdef MA_HAS_FLAC
		{ ".flac",    AssetType::AudioFile },
#endif
#ifdef MA_HAS_VORBIS
		{ ".ogg",     AssetType::AudioFile },
#endif 

		{ ".ttf",     AssetType::Font },
		{ ".otf",     AssetType::Font },

		{ ".cs",      AssetType::Script },
		{ ".sroom",   AssetType::Room },
		{ ".sobject", AssetType::Object },
	};


	using AssetRegistry = std::unordered_map<std::string, AssetMetadata>;
	using AssetMap = std::unordered_map<AssetHandle, Ref<Asset>>;
	
	using AssetImporterFunc = std::function<Ref<Asset>(const std::filesystem::path&)>;
	using AssetExporterFunc = std::function<void(Ref<Asset>, const std::filesystem::path&)>;

	class Project;

	class AssetManager
	{
	public:
		void LoadAllAssets(Ref<Project> proj = nullptr);
		void SaveAllAssets();

		bool IsAssetLoaded(AssetHandle handle) const;
		bool IsAssetLoaded(const std::filesystem::path& filepath) const;
		bool IsAssetLoaded(const std::string& name) const;
		bool IsAssetFile(AssetHandle handle) const;

		AssetHandle ImportAsset(const std::filesystem::path& filepath);
		AssetHandle CreateAsset(AssetType type);

		void SaveAsset(AssetHandle handle, const std::filesystem::path& filepath = std::filesystem::path());
		Ref<Asset> LoadAsset(const std::filesystem::path& filepath);

		Ref<Asset> GetAsset(AssetHandle handle);
		const Ref<Asset>& GetAsset(AssetHandle handle) const;
		
		const std::filesystem::path& GetFilePath(AssetHandle handle) const;
		const std::string& GetName(AssetHandle handle) const;
		AssetHandle GetHandle(const std::string& name) const;
		
		void MoveAsset(AssetHandle handle, const std::filesystem::path& filepath);
		void RemoveAsset(AssetHandle handle);
		void ReloadAsset(AssetHandle handle);

		// NOTE: Asset loading/saving functions not 
		//       accesiable in anywhere except AssetManager.cpp
		static AssetImporterFunc GetAssetImporter(AssetType type);
		static AssetExporterFunc GetAssetExporter(AssetType type);

		static AssetType GetAssetType(const std::filesystem::path& ext)
		{
			auto it = s_AssetExtensionMap.find(ext);
			return it == s_AssetExtensionMap.end() ? AssetType::None : it->second;
		}

		static std::string CalculateName(const std::filesystem::path& filepath)
		{
			std::string name = filepath.filename().string();
			auto pos = name.find('.');

			if (pos != std::string::npos)
				name = name.substr(0, pos);

			return name;
		}

		void ForEach(const std::function<void(AssetHandle)>& func)
		{
			for (const auto& [handle, asset] : m_LoadedAssets)
				func(handle);
		}
	private:
		AssetRegistry m_AssetRegistry;
		AssetMap m_LoadedAssets;

		Ref<Project> m_LoadedProject;
	};

}