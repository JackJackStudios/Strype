#include "stypch.hpp"
#include "AssetManager.hpp"

#include "AssetImporters.hpp"

#include "Strype/Core/Application.hpp"
#include "Strype/Project/Project.hpp"

namespace Strype {

    namespace Utils {

        static bool IsFileInsideFolder(const std::filesystem::path& file, const std::filesystem::path& folder)
        {
            auto absFolder = std::filesystem::weakly_canonical(folder);
            auto absFile = std::filesystem::weakly_canonical(file);
            
            // Check that absFile starts with absFolder
            return std::mismatch(absFolder.begin(), absFolder.end(), absFile.begin()).first == absFolder.end();
        }

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

    };

    void AssetManager::LoadAllAssets(Ref<Project> proj)
    {
        if (proj == nullptr) proj = m_LoadedProject;
        if (proj == nullptr)
        {
            STY_CORE_WARN("Cannot load AssetManager with no project");
            return;
        }

        using directory_iter = std::filesystem::recursive_directory_iterator;
        for (auto it = directory_iter(proj->GetConfig().ProjectDirectory); it != directory_iter(); ++it)
        {
            const auto& filepath = it->path();
            if (it->is_directory() && filepath.filename() == Project::HiddenFolder)
                it.disable_recursion_pending();

            if (s_AssetImportersMap.find(filepath.extension()) != s_AssetImportersMap.end())
                ImportAsset(filepath);
        }

        m_LoadedProject = proj;
    }

    void AssetManager::SaveAllAssets()
    {
        for (const auto& [name, metadata] : m_AssetRegistry)
            SaveAsset(metadata.Handle, metadata.Filepath);
    }

    bool AssetManager::IsAssetLoaded(AssetHandle handle) const
    {
        return m_LoadedAssets.find(handle) == m_LoadedAssets.end();
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
        std::string name = Utils::CalculateName(syspath);

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

        asset->Name = Utils::CalculateName(syspath);
        asset->Handle = handle;

        Application::Get().DispatchEvent<AssetImportedEvent>(handle);
        return handle;
    }

    Ref<Asset> AssetManager::LoadAsset(const std::filesystem::path& filepath)
    {
        // TODO: implement
        STY_CORE_VERIFY(false, "Not Implemented");
        return nullptr;
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
        // TODO: implement
        if (!IsAssetLoaded(handle))
        {
            STY_CORE_WARN("Cannot find Name for AssetHandle: {}", handle);
            return "";
        }

        return GetAsset(handle)->Name;
    }

    AssetType AssetManager::GetAssetType(AssetHandle handle) const
    {
        if (!IsAssetLoaded(handle))
        {
            STY_CORE_WARN("Cannot find Type for AssetHandle: {}", handle);
            return AssetType::None;
        }

        return GetAsset(handle)->GetType();
    }

    AssetHandle AssetManager::GetHandle(const std::string& name) const
    {
        // TODO: implement
        STY_CORE_VERIFY(false, "Not Implemented");

        return 0;
    }

    void AssetManager::SaveAsset(AssetHandle handle, const std::filesystem::path& filepath)
    {
        // TODO: implement
        STY_CORE_VERIFY(false, "Not Implemented");
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

        if (!std::filesystem::exists(GetFilePath(handle)))
        {
            STY_CORE_WARN("File not found: \"{}\"", GetFilePath(handle));
            return;
        }

        Ref<Asset> asset = LoadAsset(GetFilePath(handle));
        asset->Name = GetName(handle);
        asset->Handle = handle;

        m_LoadedAssets[handle] = asset;
    }

};