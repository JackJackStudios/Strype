#include "stypch.hpp"
#include "AssetManager.hpp"

#include "AssetSerializer.hpp"
#include "Strype/Room/RoomSerializer.hpp"
#include "Strype/Project/Project.hpp"

#define REGISTER_ASSET(a) if (Utils::GetAssetTypeFromFileExtension(path.extension()) == AssetType::a) temp = CreateRef<a>()
#define DEREGISTER_ASSET(a) STY_CORE_VERIFY(Utils::GetAssetTypeFromFileExtension(path.extension()) != AssetType::a, "Cannot create a new instance of {}", #a)

namespace Strype {

	namespace Utils {

		static std::filesystem::path ToAssetSysPath(const std::filesystem::path& filepath)
		{
			return filepath.is_absolute() ? std::filesystem::relative(filepath, Project::GetProjectDirectory()) : filepath;
		}

		const std::filesystem::path& GetFileExtensionFromAssetType(AssetType type)
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

	void AssetManager::NewAsset(const std::filesystem::path& path)
	{
		Ref<Asset> temp = nullptr;

		REGISTER_ASSET(Object);
		REGISTER_ASSET(Room);

		// TODO: Remove sprite when added animations
		DEREGISTER_ASSET(Sprite);
		DEREGISTER_ASSET(AudioFile);

		m_Serializers[temp->GetType()]->SaveAsset(temp, Project::GetProjectDirectory() / Utils::ToAssetSysPath(path));
		ImportAsset(Utils::ToAssetSysPath(path));
	}

	AssetManager::AssetManager()
	{
		m_Serializers.clear();
		m_Serializers[AssetType::Object] = CreateScope<PrefabSerializer>();
		m_Serializers[AssetType::Sprite] = CreateScope<SpriteSerializer>();
		m_Serializers[AssetType::Room] = CreateScope<RoomSerializer>();
		m_Serializers[AssetType::AudioFile] = CreateScope<AudioFileSerializer>();
	}

	AssetManager::~AssetManager()
	{
		for (auto& [handle, asset] : m_AssetRegistry)
			asset.reset();
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
			asset = m_AssetRegistry.at(handle).asset;
		}
		else
		{
			const AssetMetadata& metadata = GetMetadata(handle);
			asset = ImportAsset(handle, metadata);

			m_AssetRegistry[handle].asset = asset;
			m_AssetRegistry[handle].Type = asset->GetType();
		}

		return asset;
	}

	bool AssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return handle != 0 && m_AssetRegistry.find(handle) != m_AssetRegistry.end();
	}

	bool AssetManager::IsAssetLoaded(const std::filesystem::path& filepath) const
	{
		return m_LoadedFiles.find(Utils::ToAssetSysPath(filepath)) != m_LoadedFiles.end();
	}

	const AssetType AssetManager::GetAssetType(AssetHandle handle) const
	{
		if (m_AssetRegistry.find(handle) == m_AssetRegistry.end())
		{
			STY_CORE_WARN("Could not find AssetType for ({}) ", (uint64_t)handle);
			return AssetType::None;
		}

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
			STY_CORE_WARN("Could not find AssetHandle for \"{}\" ", Utils::ToAssetSysPath(path));
			return 0;
		}

		return m_LoadedFiles.at(Utils::ToAssetSysPath(path));
	}

	void AssetManager::SaveAsset(AssetHandle handle, const std::filesystem::path& path)
	{
		if (m_Serializers.find(GetAssetType(handle)) == m_Serializers.end())
		{
			STY_CORE_WARN("No serializer available for Type: {}", magic_enum::enum_name(GetAssetType(handle)));
			return;
		}

		STY_CORE_TRACE("Serializing asset \"{}\" ", GetFilePath(handle));

		m_Serializers[GetAssetType(handle)]->SaveAsset(GetAsset(handle), Project::GetProjectDirectory() / Utils::ToAssetSysPath(path));
	}

    void AssetManager::DeleteAsset(AssetHandle handle)
    {
		if (m_LoadedFiles.find(GetFilePath(handle)) == m_LoadedFiles.end())
		{
			STY_CORE_WARN("Cannot delete memory-only assets from the filesystem");
			return;
		}

		STY_CORE_TRACE("Deleting asset \"{}\" ", GetFilePath(handle));

		std::filesystem::path path = GetFilePath(handle);
		m_AssetRegistry[handle].reset();

		m_AssetRegistry.erase(handle);
		m_LoadedFiles.erase(path);

		//std::filesystem::remove(Project::GetProjectDirectory() / path);
    }

    AssetSerializer* AssetManager::GetSerializer(AssetType type)
	{
		if (m_Serializers.find(type) == m_Serializers.end())
		{
			STY_CORE_WARN("No serializer available for Type: {}", magic_enum::enum_name(type));
			return nullptr;
		}

		return m_Serializers[type].get();
	}

	AssetHandle AssetManager::ImportAsset(const std::filesystem::path& filepath)
	{
		if (IsAssetLoaded(Utils::ToAssetSysPath(filepath))) return GetHandle(Utils::ToAssetSysPath(filepath));

		AssetHandle handle;
		AssetMetadata metadata;
		metadata.FilePath = Utils::ToAssetSysPath(filepath);
		metadata.Type = Utils::GetAssetTypeFromFileExtension(filepath.extension());

		if (metadata.Type == AssetType::None)
		{
			STY_CORE_WARN("Could not import Asset \"{}\" ", filepath);
			return 0;
		}

		Ref<Asset> asset = ImportAsset(handle, metadata);

		if (asset)
		{
			asset->Handle = handle;
			metadata.asset = asset;

			m_AssetRegistry[handle] = metadata;
			m_LoadedFiles[metadata.FilePath] = handle;
		}
		else
		{
			STY_CORE_WARN("Asset import failed \"{}\" ", metadata.FilePath);
		}

		return handle;
	}

	Ref<Asset> AssetManager::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		if (m_Serializers.find(metadata.Type) == m_Serializers.end())
		{
			STY_CORE_WARN("No serializer available for Type: {}", magic_enum::enum_name(metadata.Type));
			return nullptr;
		}

		STY_CORE_TRACE("Deserializing asset \"{}\" ", metadata.FilePath);

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


