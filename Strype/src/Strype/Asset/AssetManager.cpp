#include "stypch.hpp"
#include "AssetManager.hpp"

#include "AssetSerializer.hpp"
#include "Strype/Room/RoomSerializer.hpp"
#include "Strype/Project/Project.hpp"
#include "Strype/Core/Application.hpp"

#define REGISTER_ASSET(a) if (type == AssetType::a) { if (createAsset) { createdAsset = CreateRef<a>(); } return true; }
#define DEREGISTER_ASSET(a) if (type == AssetType::a) return false

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

	bool AssetManager::CanCreateAsset(AssetType type, bool createAsset, Ref<Asset>& createdAsset) const
	{
		REGISTER_ASSET(Room);

		// TODO: Create object with template
		DEREGISTER_ASSET(Object);

		// TODO: Remove sprite when added animations
		DEREGISTER_ASSET(Sprite);
		DEREGISTER_ASSET(AudioFile);

		STY_CORE_VERIFY(false, "Unknown AssetType");
		return false;
	}

	void AssetManager::CreateAsset(const std::filesystem::path& path)
	{
		Ref<Asset> temp = nullptr;
		AssetType type = Utils::GetAssetTypeFromFileExtension(path.extension());
		
		bool canCreate = CanCreateAsset(type, true, temp);
		if (!canCreate || temp == nullptr)
		{
			STY_CORE_WARN("Cannot create new instance of {}", magic_enum::enum_name(type));
		}

		m_Serializers[temp->GetType()]->SaveAsset(temp, Project::GetProjectDirectory() / Utils::ToAssetSysPath(path));
		ImportAsset(Utils::ToAssetSysPath(path));
	}

	AssetManager::AssetManager()
	{
		m_Serializers.clear();
		m_Serializers[AssetType::Object] = CreateScope<ObjectSerializer>();
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
		return m_AssetRegistry.at(handle);
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

		return m_AssetRegistry.at(handle)->GetType();
	}

	const std::filesystem::path& AssetManager::GetFilePath(AssetHandle handle)
	{
		return GetAsset(handle)->FilePath;
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

    void AssetManager::RemoveAsset(AssetHandle handle)
    {
		if (m_LoadedFiles.find(GetFilePath(handle)) == m_LoadedFiles.end())
		{
			STY_CORE_WARN("Cannot delete memory-only assets from the filesystem");
			return;
		}

		STY_CORE_TRACE("Removing asset from AssetManager: \"{}\" ", GetFilePath(handle));

		AssetRemovedEvent e(handle);
		Application::Get().OnEvent(e);

		std::filesystem::path path = GetFilePath(handle);
		m_AssetRegistry[handle].reset();

		m_AssetRegistry.erase(handle);
		m_LoadedFiles.erase(path);
    }

	void AssetManager::MoveAsset(AssetHandle handle, const std::filesystem::path& path)
	{
		const auto& newpath = Utils::ToAssetSysPath(path);
		if (std::filesystem::exists(Project::GetProjectDirectory() / newpath))
		{
			STY_CORE_WARN("Cannot override other files when moving asset \"{}\" ", path);
			return;
		}

		if (GetFilePath(handle).extension() != newpath.extension())
		{
			STY_CORE_WARN("Cannot change file extension, this may corrupt data! ({} -> {})", GetFilePath(handle).extension(), newpath.extension());
			return;
		}

		AssetMovedEvent e(handle, newpath);
		Application::Get().OnEvent(e);

		m_LoadedFiles.erase(GetFilePath(handle));
		m_LoadedFiles[newpath] = handle;

		m_AssetRegistry[handle]->FilePath = newpath;
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
		std::filesystem::path path = Utils::ToAssetSysPath(filepath);

		if (Utils::GetAssetTypeFromFileExtension(filepath.extension()) == AssetType::None)
		{
			STY_CORE_WARN("Could not import Asset \"{}\" ", filepath);
			return 0;
		}

		Ref<Asset> asset = ImportAsset(handle, path);

		if (asset)
		{
			asset->Handle = handle;
			asset->FilePath = path;

			m_AssetRegistry[handle] = asset;
			m_LoadedFiles[path] = handle;

			AssetImportedEvent e(handle);
			Application::Get().OnEvent(e);
		}
		else
		{
			STY_CORE_WARN("Asset import failed \"{}\" ", path);
		}

		return handle;
	}

	Ref<Asset> AssetManager::ImportAsset(AssetHandle handle, const std::filesystem::path& filepath)
	{
		AssetType type = Utils::GetAssetTypeFromFileExtension(filepath.extension());

		if (m_Serializers.find(type) == m_Serializers.end())
		{
			STY_CORE_WARN("No serializer available for Type: {}", magic_enum::enum_name(type));
			return nullptr;
		}

		STY_CORE_TRACE("Deserializing asset \"{}\" ", filepath);
		return m_Serializers.at(type)->LoadAsset(Project::GetProjectDirectory() / filepath);
	}

	void AssetManager::LoadDirectory(const std::filesystem::path& path)
	{
		for (auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory() && entry.path().filename() != HIDDEN_FOLDER)
				LoadDirectory(entry.path());
			else if (s_AssetExtensionMap.find(entry.path().extension()) != s_AssetExtensionMap.end())
				ImportAsset(std::filesystem::relative(entry.path(), Project::GetProjectDirectory()));
		}
	}

}


