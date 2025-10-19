#include "stypch.hpp"
#include "AssetManager.hpp"

#include "AssetImporters.hpp"

#include "Strype/Core/Application.hpp"
#include "Strype/Project/Project.hpp"

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

        static std::string GetFullExtension(const std::filesystem::path& path)
        {
            std::string filename = path.filename().string();
            size_t firstDot = filename.find('.');
            if (firstDot == std::string::npos)
                return "";

            return filename.substr(firstDot);
        }

    };

    void AssetManager::LoadAllAssets(Ref<Project> proj)
    {
        if (proj == nullptr) proj = m_LoadedProject;
        if (proj == nullptr)
        {
            STY_CORE_WARN("Cannot load AssetManager with no project");
            return;
        }

        m_LoadedProject = proj;

        using directory_iter = std::filesystem::recursive_directory_iterator;
        for (auto it = directory_iter(proj->GetConfig().ProjectDirectory); it != directory_iter(); ++it)
        {
            const auto& filepath = it->path();
            if (it->is_directory() && filepath.filename() == Project::HiddenFolder)
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

    bool AssetManager::IsAssetLoaded(AssetHandle handle) const
    {
        return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
    }

    bool AssetManager::IsAssetLoaded(const std::filesystem::path& filepath) const
    {
        auto it = m_AssetRegistry.find(CalculateName(filepath));
        if (it == m_AssetRegistry.end()) return false;
        
        return IsAssetLoaded(it->second.Handle);
    }

    bool AssetManager::IsAssetFile(AssetHandle handle) const
    {
        return IsAssetLoaded(handle) && m_AssetRegistry.find(GetAsset(handle)->Name) != m_AssetRegistry.end();
    }

    Ref<Asset> AssetManager::GetAsset(AssetHandle handle)
    {
        auto it = m_LoadedAssets.find(handle);
        if (m_LoadedAssets.end() == it)
        {
            STY_CORE_WARN("Cannot find Asset for AssetHandle: {}", handle);
            return nullptr;
        }

        return it->second;
    }

    const Ref<Asset>& AssetManager::GetAsset(AssetHandle handle) const
    {
        auto it = m_LoadedAssets.find(handle);
        if (m_LoadedAssets.end() == it)
        {
            STY_CORE_WARN("Cannot find Asset for AssetHandle: {}", handle);
            return nullptr;
        }

        return it->second;
    }

    AssetHandle AssetManager::ImportAsset(const std::filesystem::path& filepath)
    {
        AssetHandle handle; // Randomly generated UUID

        std::filesystem::path syspath = Utils::ToAssetSysPath(filepath);
        std::string name = CalculateName(syspath);

        if (!std::filesystem::exists(m_LoadedProject->GetConfig().ProjectDirectory / syspath))
        {
            STY_CORE_WARN("File not found: \"{}\" ", syspath);
            return 0;
        }

        Ref<Asset> asset = LoadAsset(syspath);
        if (asset == nullptr)
        {
            STY_CORE_WARN("Asset import failed: \"{}\" ", filepath);
            return 0;
        }

        AssetMetadata metadata;
        metadata.Handle = handle;
        metadata.Filepath = syspath;

        m_AssetRegistry[name] = metadata;
        m_LoadedAssets[handle] = asset;

        asset->Name = name;
        asset->Handle = handle;

        Application::Get().DispatchEvent<AssetImportedEvent>(handle);
        return handle;
    }

    AssetHandle AssetManager::CreateAsset(AssetType type)
    {
        // TODO: implement
        STY_CORE_VERIFY(false, "Not Implemented");
        return 0;
    }

    Ref<Asset> AssetManager::LoadAsset(const std::filesystem::path& filepath)
    {
        if (!std::filesystem::exists(m_LoadedProject->GetConfig().ProjectDirectory / Utils::ToAssetSysPath(filepath)))
        {
            STY_CORE_WARN("File not found: \"{}\" ", Utils::ToAssetSysPath(filepath));
            return nullptr;
        }

        AssetType type = AssetManager::GetAssetType(filepath.extension());
        if (s_AssetImportersMap.find(type) == s_AssetImportersMap.end())
        {
            STY_CORE_WARN("No importer available for AssetType::{}", magic_enum::enum_name(type));
            return nullptr;
        }

        return s_AssetImportersMap[type](m_LoadedProject->GetConfig().ProjectDirectory / Utils::ToAssetSysPath(filepath));
    }

    const std::filesystem::path& AssetManager::GetFilePath(AssetHandle handle) const
    {
        if (!IsAssetFile(handle))
        {
            STY_CORE_WARN("Cannot find Filepath for AssetHandle: {}", handle);
            return "";
        }

        return m_AssetRegistry.at(GetName(handle)).Filepath;
    }

    const std::string& AssetManager::GetName(AssetHandle handle) const
    {
        if (!IsAssetLoaded(handle))
        {
            STY_CORE_WARN("Cannot find Name for AssetHandle: {}", handle);
            return "";
        }

        return GetAsset(handle)->Name;
    }

    AssetHandle AssetManager::GetHandle(const std::string& name) const
    {
        auto it = m_AssetRegistry.find(name);
        if (it == m_AssetRegistry.end())
        {
            STY_CORE_WARN("Cannot find AssetHandle for Name: {}", name);
            return 0;
        }

        return it->second.Handle;
    }

    void AssetManager::SaveAsset(AssetHandle handle, const std::filesystem::path& filepath)
    {
        if (!IsAssetFile(handle))
        {
            STY_CORE_WARN("Cannot save memory-only AssetHandle: {}", handle);
            return;
        }

        Ref<Asset> asset = GetAsset(handle);

        auto it = s_AssetExportersMap.find(asset->GetType());
        if (it == s_AssetExportersMap.end())
        {
            STY_CORE_WARN("No exporter available for AssetType::{}", magic_enum::enum_name(asset->GetType()));
            return;
        }

        it->second(asset, Project::GetProjectDirectory() / (filepath.empty() ? GetFilePath(handle) : Utils::ToAssetSysPath(filepath)));
    }

    void AssetManager::MoveAsset(AssetHandle handle, const std::filesystem::path& destPath)
    {
        if (!IsAssetFile(handle))
        {
            STY_CORE_WARN("Cannot move/rename memory-only AssetHandle: {}", handle);
            return;
        }

        const auto oldPath = GetFilePath(handle);
        auto newPath = Utils::ToAssetSysPath(destPath);

        if (!newPath.has_filename())
            newPath /= oldPath.filename();

        if (!newPath.has_extension())
            newPath.replace_extension(Utils::GetFullExtension(oldPath));

        auto fullNewPath = m_LoadedProject->GetConfig().ProjectDirectory / newPath;
        if (std::filesystem::exists(fullNewPath))
        {
            STY_CORE_WARN("Cannot move AssetHandle {} - destination already exists: {}", handle, newPath);
            return;
        }

        if (newPath.extension() != oldPath.extension())
        {
            STY_CORE_WARN("Cannot change file extension when moving AssetHandle {}", handle);
            return;
        }

        std::filesystem::rename(m_LoadedProject->GetConfig().ProjectDirectory / oldPath, fullNewPath);
        Application::Get().DispatchEvent<AssetMovedEvent>(handle, newPath);

        const auto& oldName = GetName(handle);
        const auto newName = CalculateName(newPath);

        AssetMetadata& metadata = m_AssetRegistry[oldName];
        metadata.Filepath = newPath;

        if (newName != oldName)
        {
            m_AssetRegistry[newName] = metadata;
            m_AssetRegistry.erase(oldName);
        }
    }

    void AssetManager::RemoveAsset(AssetHandle handle)
    {
        if (!IsAssetLoaded(handle))
        {
            STY_CORE_WARN("Cannot remove Asset for AssetHandle: {}", handle);
            return;
        }

        Application::Get().DispatchEvent<AssetRemovedEvent>(handle);

        if (IsAssetFile(handle)) m_AssetRegistry.erase(GetName(handle));
        m_LoadedAssets.erase(handle);
    }

    void AssetManager::ReloadAsset(AssetHandle handle)
    {
        if (!IsAssetFile(handle))
        {
            STY_CORE_WARN("Cannot reload memory-only AssetHandle: {}", handle);
            return;
        }

        if (!std::filesystem::exists(Project::GetProjectDirectory() / GetFilePath(handle)))
        {
            STY_CORE_WARN("File not found: \"{}\"", GetFilePath(handle));
            return;
        }

        Ref<Asset> asset = LoadAsset(GetFilePath(handle));
        asset->Name = GetName(handle);
        asset->Handle = handle;

        m_LoadedAssets[handle] = asset;
    }

    AssetImporterFunc AssetManager::GetAssetImporter(AssetType type)
    {
        auto it = s_AssetImportersMap.find(type);
        return it == s_AssetImportersMap.end() ? nullptr : it->second;
    }

    AssetExporterFunc AssetManager::GetAssetExporter(AssetType type)
    {
        auto it = s_AssetExportersMap.find(type);
        return it == s_AssetExportersMap.end() ? nullptr : it->second;
    }

};