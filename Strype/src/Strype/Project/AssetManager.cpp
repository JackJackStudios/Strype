#include "stypch.hpp"
#include "AssetManager.hpp"

#include "AssetSerializer.hpp"
#include "Strype/Project/Project.hpp"
#include "Strype/Core/Application.hpp"

#define REGISTER_ASSET(a) if (type == AssetType::a) { if (createAsset) { createdAsset = CreateRef<a>(); } return true; }
#define DEREGISTER_ASSET(a) if (type == AssetType::a) return false

namespace Strype {

	namespace Utils {

		static std::filesystem::path ToAssetSysPath(const std::filesystem::path& filepath)
		{
			if (filepath.is_absolute() && Utils::IsFileInsideFolder(filepath, Project::GetProjectDirectory()))
			{
				return std::filesystem::relative(filepath, Project::GetProjectDirectory());
			}

			return filepath;
		}

		static std::string CalculateName(const std::filesystem::path& filepath)
		{
			std::string name = filepath.filename().string();
			auto pos = name.find('.');

			if (pos != std::string::npos)
				name = name.substr(0, pos);

			return name;
		}

	}

	bool AssetManager::CanCreateAsset(AssetType type, bool createAsset, Ref<Asset>& createdAsset) const
	{
		REGISTER_ASSET(Room);
		REGISTER_ASSET(Object);
		REGISTER_ASSET(Script);

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
			return;
		}

		m_Serializers[temp->GetType()]->SaveAsset(temp, Project::GetProjectDirectory() / Utils::ToAssetSysPath(path));
		ImportAsset(Utils::ToAssetSysPath(path));
	}

	AssetManager::AssetManager()
	{
		m_Serializers.clear();
		SetSerializers();
	}

	AssetManager::~AssetManager()
	{
		for (auto& [handle, asset] : m_LoadedAssets)
			asset.reset();
	}

	AssetHandle AssetManager::ImportAsset(std::filesystem::path filepath, AssetHandle handle)
	{
		filepath = Utils::ToAssetSysPath(filepath);

		std::string name = Utils::CalculateName(filepath);
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
			asset->Name = name;
			m_LoadedAssets[handle] = asset;

			AssetMetadata metadata;
			metadata.Handle = handle;
			metadata.Filepath = filepath;
			m_AssetRegistry[name] = metadata;

			Application::Get().DispatchEvent<AssetImportedEvent>(handle);
		}
		else
		{
			STY_CORE_WARN("Asset import failed: \"{}\" ", filepath);
			return 0;
		}

		return handle;
	}

	Ref<Asset> AssetManager::LoadAsset(const std::filesystem::path& filepath)
	{
		AssetType type = Utils::GetAssetTypeFromFileExtension(filepath.extension());

		auto it = m_Serializers.find(type);
		if (it == m_Serializers.end())
		{
			STY_CORE_WARN("No serializer available for AssetType::{}", magic_enum::enum_name(type));
			return nullptr;
		}

		STY_CORE_TRACE("Loading asset \"{}\" ", filepath);
		return it->second->LoadAsset(Project::GetProjectDirectory() / filepath);
	}

	AssetType AssetManager::GetAssetType(AssetHandle handle) const
	{
		auto it = m_LoadedAssets.find(handle);
		if (it == m_LoadedAssets.end())
		{
			STY_CORE_WARN("Could not find AssetType for ({}) ", handle);
			return AssetType::None;
		}

		return it->second->GetType();
	}

	AssetHandle AssetManager::GetHandle(const std::string& name) const
	{
		auto it = m_AssetRegistry.find(name);
		if (it == m_AssetRegistry.end())
		{
			STY_CORE_WARN("Could not find AssetHandle for \"{}\" ", name);
			return 0;
		}

		return it->second.Handle;
	}

	AssetHandle AssetManager::GetHandle(const std::filesystem::path& filepath) const
	{
		auto it = m_AssetRegistry.find(Utils::CalculateName(filepath));

		if (!IsAssetLoaded(filepath) || it->second.Filepath != Utils::ToAssetSysPath(filepath))
		{
			STY_CORE_WARN("Cannot find Handle for \"{}\" ", Utils::ToAssetSysPath(filepath));
		}

		return it->second.Handle;
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
		STY_CORE_TRACE("Removing asset: \"{}\" ", GetFilePath(handle));

		const std::string& name = GetName(handle);

		if (m_AssetRegistry.find(name) != m_AssetRegistry.end())
			m_AssetRegistry.erase(name);

		m_LoadedAssets[handle].reset();
		m_LoadedAssets.erase(handle);

		Application::Get().DispatchEvent<AssetRemovedEvent>(handle);
    }

	void AssetManager::ReloadAsset(AssetHandle handle)
	{
		if (!IsAssetLoaded(handle))
		{
			STY_CORE_WARN("Cannot find Filepath for AssetHandle: {}", handle);
			return;
		}

		auto& filepath = GetFilePath(handle);
		Ref<Asset> newAsset = LoadAsset(filepath);
		if (!newAsset)
		{
			STY_CORE_WARN("Cannot reload asset \"{}\" ", filepath);
			return;
		}

		newAsset->Handle = handle;
		newAsset->Name = filepath.stem().string();
		m_LoadedAssets[handle] = newAsset;
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

		Application::Get().DispatchEvent<AssetMovedEvent>(handle, newpath);

		AssetMetadata metadata;
		metadata.Handle = handle;
		metadata.Filepath = newpath;
		m_AssetRegistry[GetName(handle)] = metadata;
	}

	Ref<Asset>& AssetManager::GetAsset(AssetHandle handle)
	{
		static Ref<Asset> emptyAsset = nullptr;
		if (!IsAssetLoaded(handle))
		{
			STY_CORE_WARN("Cannot find Asset for handle: {}", handle);
			return emptyAsset;
		}

		return m_LoadedAssets.at(handle);
	}

	bool AssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return handle != 0 && m_LoadedAssets.find(handle) != m_LoadedAssets.end();
	}

	const std::filesystem::path& AssetManager::GetFilePath(AssetHandle handle) const
	{
		static std::filesystem::path emptyPath = "";
		if (!IsAssetLoaded(handle))
		{
			STY_CORE_WARN("Cannot find Filepath for handle: {}", handle);
			return emptyPath;
		}

		return m_AssetRegistry.at(GetName(handle)).Filepath;
	}

	const std::string& AssetManager::GetName(AssetHandle handle) const
	{
		if (!IsAssetLoaded(handle))
		{
			STY_CORE_WARN("Cannot find Name for handle: {}", handle);
			return "";
		}

		return m_LoadedAssets.at(handle)->Name;
	}

	bool AssetManager::IsAssetLoaded(const std::string& name) const
	{
		return m_AssetRegistry.find(name) != m_AssetRegistry.end();
	}

	bool AssetManager::IsAssetLoaded(const std::filesystem::path& filepath) const
	{
		if (!IsAssetLoaded(filepath.stem().string()))
		{
			return false;
		}

		AssetHandle handle = GetHandle(filepath.stem().string());
		if (filepath.has_extension())
		{
			if (GetAssetType(handle) != Utils::GetAssetTypeFromFileExtension(filepath.extension()))
			{
				return false;
			}
		}

		return GetFilePath(handle) == Utils::ToAssetSysPath(filepath);
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
		for (const auto& [name, metadata] : m_AssetRegistry)
			SaveAsset(metadata.Handle, metadata.Filepath);
	}

};