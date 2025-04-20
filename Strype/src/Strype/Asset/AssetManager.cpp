#include "stypch.h"
#include "AssetManager.h"

#include "Strype/Project/Project.h"

#include "SpriteSerializer.h"
#include "Strype/Room/PrefabSerializer.h"
#include "Strype/Room/RoomSerializer.h"
#include "Strype/Audio/AudioFileSerializer.h"

namespace Strype {

	namespace Utils {

		std::filesystem::path ToAssetSysPath(const std::filesystem::path& filepath)
		{
			return filepath.is_absolute() ? std::filesystem::relative(filepath, Project::GetProjectDirectory()) : filepath;
		}

	}

	AssetManager::AssetManager()
	{
		m_Serializers.clear();
		m_Serializers[AssetType::Prefab] = CreateScope<PrefabSerializer>();
		m_Serializers[AssetType::Sprite] = CreateScope<SpriteSerializer>();
		m_Serializers[AssetType::Room] = CreateScope<RoomSerializer>();
		m_Serializers[AssetType::AudioFile] = CreateScope<AudioFileSerializer>();
	}
	
	void AssetManager::ReloadAssets()
	{
		LoadDirectory(Project::GetProjectDirectory());
	}

	void AssetManager::SaveAllAssets()
	{
		for (const auto& [path, handle] : m_LoadedFiles)
			SaveAsset(handle, Project::GetProjectDirectory() / path);
	}

	Ref<Asset> AssetManager::GetAsset(AssetHandle handle)
	{
		Ref<Asset> asset;
		if (IsAssetLoaded(handle))
		{
			asset = m_LoadedAssets.at(handle);
		}
		else
		{
			const AssetMetadata& metadata = GetMetadata(handle);
			asset = ImportAsset(handle, metadata);

			m_LoadedAssets[handle] = asset;
		}

		return asset;
	}

	bool AssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
	}

	bool AssetManager::IsAssetLoaded(const std::filesystem::path& filepath) const
	{
		return m_LoadedFiles.find(Utils::ToAssetSysPath(filepath)) != m_LoadedFiles.end();
	}

	const AssetType AssetManager::GetAssetType(AssetHandle handle) const
	{
		return m_AssetRegistry.at(handle).Type;
	}

	const std::filesystem::path& AssetManager::GetFilePath(AssetHandle handle) const
	{
		return GetMetadata(handle).FilePath;
	}

	AssetHandle AssetManager::GetHandle(const std::filesystem::path& path) const
	{
		if (m_LoadedFiles.find(Utils::ToAssetSysPath(path)) == m_LoadedFiles.end())
		{
			STY_CORE_WARN("Could not find AssetHandle for \"{}\" ", Utils::ToAssetSysPath(path).string());
			return 0;
		}

		return m_LoadedFiles.at(Utils::ToAssetSysPath(path));
	}

	void AssetManager::SaveAsset(AssetHandle handle, const std::filesystem::path& path)
	{
		m_Serializers[GetAssetType(handle)]->SaveAsset(GetAsset(handle), path);
	}

	AssetHandle AssetManager::ImportAsset(const std::filesystem::path& filepath)
	{
		if (IsAssetLoaded(Utils::ToAssetSysPath(filepath))) return GetHandle(Utils::ToAssetSysPath(filepath));

		AssetHandle handle;
		AssetMetadata metadata;
		metadata.Handle = handle;
		metadata.FilePath = Utils::ToAssetSysPath(filepath);
		metadata.Type = Utils::GetAssetTypeFromFileExtension(filepath.extension());

		if (metadata.Type == AssetType::None)
		{
			STY_CORE_WARN("Could not import Asset \"{}\" ", filepath.string());
			return 0;
		}

		Ref<Asset> asset = ImportAsset(handle, metadata);

		if (asset)
		{
			asset->Handle = handle;
			m_LoadedAssets[handle] = asset;
			m_LoadedFiles[metadata.FilePath] = handle;
			m_AssetRegistry[handle] = metadata;
		}
		else
		{
			STY_CORE_WARN("Asset import failed \"{}\" ", metadata.FilePath.string());
		}

		return handle;
	}

	Ref<Asset> AssetManager::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		if (m_Serializers.find(metadata.Type) == m_Serializers.end())
		{
			STY_CORE_WARN("No serializer available for Type: {}", Utils::AssetTypeToString(metadata.Type));
			return nullptr;
		}

		STY_CORE_TRACE("Deserializing asset \"{}\" ", metadata.FilePath.string());

		return m_Serializers.at(metadata.Type)->LoadAsset(Project::GetProjectDirectory() / metadata.FilePath);
	}

	const AssetMetadata& AssetManager::GetMetadata(AssetHandle handle) const
	{
		if (m_AssetRegistry.find(handle) == m_AssetRegistry.end())
		{
			STY_CORE_WARN("Could not find AssetMetadata for {}", (uint64_t)handle);
			return {};
		}

		return m_AssetRegistry.at(handle);
	}

	void AssetManager::LoadDirectory(const std::filesystem::path& path)
	{
		for (auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory())
				LoadDirectory(entry.path());
			else if (s_AssetExtensionMap.find(entry.path().extension()) != s_AssetExtensionMap.end())
				ImportAsset(std::filesystem::relative(entry.path(), Project::GetProjectDirectory()));
		}
	}

}


