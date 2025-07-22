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

		static bool IsFileInsideFolder(const std::filesystem::path& file, const std::filesystem::path& folder)
		{
			// For this function to return true, the file must exist in the folder and it exists

			auto absFile = std::filesystem::absolute(file).lexically_normal();
			auto absFolder = std::filesystem::absolute(folder).lexically_normal();

			return std::mismatch(absFolder.begin(), absFolder.end(), absFile.begin()).first == absFolder.end() && std::filesystem::exists(file);
		}

		static std::filesystem::path ToAssetSysPath(const std::filesystem::path& filepath)
		{
			if (filepath.is_absolute() && Utils::IsFileInsideFolder(filepath, Project::GetProjectDirectory()))
			{
				return std::filesystem::relative(filepath, Project::GetProjectDirectory());
			}

			return filepath;
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

	AssetHandle AssetManager::ImportAsset(std::filesystem::path filepath, AssetHandle handle)
	{
		if (filepath.is_absolute() && Utils::IsFileInsideFolder(filepath, Project::GetProjectDirectory()))
			filepath = std::filesystem::relative(filepath, Project::GetProjectDirectory());

		auto name = filepath.parent_path() / filepath.stem();
		if (IsAssetLoaded(name) && !IsAssetLoaded(handle)) return GetHandle(name);

		if (!std::filesystem::exists(Project::GetProjectDirectory() / filepath))
		{
			STY_CORE_WARN("File not found: \"{}\" ", filepath);
			return 0;
		}

		Ref<Asset> asset = LoadAsset(filepath);

		if (asset)
		{
			asset->Handle = handle;
			asset->FilePath = filepath;

			m_AssetRegistry[handle] = asset;
			m_LoadedFiles[name] = handle;

			AssetImportedEvent e(handle);
			Application::Get().OnEvent(e);
		}
		else
		{
			AssetType type = Utils::GetAssetTypeFromFileExtension(filepath.extension());

			STY_CORE_WARN("Asset import failed AssetType::{}: \"{}\" ", magic_enum::enum_name(type), filepath);
			return 0;
		}

		return handle;
	}

	Ref<Asset> AssetManager::LoadAsset(const std::filesystem::path& filepath)
	{
		AssetType type = Utils::GetAssetTypeFromFileExtension(filepath.extension());

		if (m_Serializers.find(type) == m_Serializers.end())
		{
			STY_CORE_WARN("No serializer available for AssetType::{}", magic_enum::enum_name(type));
			return nullptr;
		}

		STY_CORE_TRACE("Loading asset \"{}\" ", filepath);
		return m_Serializers.at(type)->LoadAsset(Project::GetProjectDirectory() / filepath);
	}

	AssetType AssetManager::GetAssetType(AssetHandle handle) const
	{
		if (m_AssetRegistry.find(handle) == m_AssetRegistry.end())
		{
			STY_CORE_WARN("Could not find AssetType for ({}) ", (uint64_t)handle);
			return AssetType::None;
		}

		return m_AssetRegistry.at(handle)->GetType();
	}

	AssetHandle AssetManager::GetHandle(const std::filesystem::path& path) const
	{
		if (path.has_extension())
		{
			STY_CORE_WARN("Filepaths entered into AssetManager shouldn't have a extension \"{}\" ", path.extension());
			return GetHandle(path.parent_path() / path.stem());
		}

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
			STY_CORE_WARN("No serializer available for AssetType::{}", magic_enum::enum_name(GetAssetType(handle)));
			return;
		}

		STY_CORE_TRACE("Saving asset \"{}\" ", Utils::ToAssetSysPath(path));
		m_Serializers[GetAssetType(handle)]->SaveAsset(GetAsset(handle), Project::GetProjectDirectory() / Utils::ToAssetSysPath(path));
	}

    void AssetManager::RemoveAsset(AssetHandle handle)
    {
		STY_CORE_TRACE("Removing asset from AssetManager: \"{}\" ", GetFilePath(handle));

		AssetRemovedEvent e(handle);
		Application::Get().OnEvent(e);

		std::filesystem::path path = GetFilePath(handle);

		m_AssetRegistry[handle].reset();
		m_AssetRegistry.erase(handle);

		if (m_LoadedFiles.find(GetFilePath(handle)) != m_LoadedFiles.end())
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

	Ref<Asset> AssetManager::GetAsset(AssetHandle handle)
	{
		return m_AssetRegistry.at(handle);
	}

	bool AssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return handle != 0 && m_AssetRegistry.find(handle) != m_AssetRegistry.end();
	}

	const std::filesystem::path& AssetManager::GetFilePath(AssetHandle handle)
	{
		return GetAsset(handle)->FilePath;
	}

	bool AssetManager::IsAssetLoaded(const std::filesystem::path& filepath) const
	{
		return m_LoadedFiles.find(Utils::ToAssetSysPath(filepath)) != m_LoadedFiles.end();
	}

	AssetSerializer* AssetManager::GetSerializer(AssetType type)
	{
		if (m_Serializers.find(type) == m_Serializers.end())
		{
			STY_CORE_WARN("No serializer available for AssetType::{}", magic_enum::enum_name(type));
			return nullptr;
		}

		return m_Serializers[type].get();
	}

	void AssetManager::ReloadAssets()
	{
		using directory_iter = std::filesystem::recursive_directory_iterator;
		for (auto it = directory_iter(Project::GetProjectDirectory()); it != directory_iter(); ++it)
		{
			const auto& filepath = it->path();
			if (it->is_directory() && filepath.filename() == HIDDEN_FOLDER)
				it.disable_recursion_pending();

			if (s_AssetExtensionMap.find(filepath.extension()) != s_AssetExtensionMap.end())
				ImportAsset(filepath);
		}
	}

	void AssetManager::SaveAllAssets()
	{
		for (const auto& [handle, asset] : m_AssetRegistry)
			SaveAsset(handle, Project::GetProjectDirectory() / asset->FilePath);
	}

}


