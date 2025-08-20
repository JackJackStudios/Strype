#pragma once

#include "AssetManager.hpp"
#include "Strype/Core/Input.hpp"

namespace Strype {

	class ScriptEngine;
	class Room;

	struct ProjectConfig
	{
		std::string Name;
		glm::ivec2 ViewportSize;
		AGI::WindowProps RuntimeProps;

		std::string StartRoom;
		std::filesystem::path ProjectDirectory;
	};

	// Relative to working directory
	constexpr std::string_view EMPTY_PROJECT = "assets/EmptyProject/EmptyProject.sproj";
	constexpr std::string_view HIDDEN_FOLDER = "strype";

	//Note: Project is not a Asset because it holds the AssetManager
	class Project
	{
	public:
		Project();
		~Project();

		const ProjectConfig& GetConfig() const { return m_Config; }

		static Ref<AssetManager> GetAssetManager() { return GetActive()->m_AssetManager; }
		static Ref<ScriptEngine>& GetScriptEngine() { return GetActive()->m_ScriptEngine; }
		static Room* GetActiveRoom() { return GetActive()->m_ActiveRoom; }

		static void SetActiveRoom(Room* room) { GetActive()->m_ActiveRoom = room; }

		static Ref<Project> GetActive() { return s_ActiveProject; }
		static void SetActive(Ref<Project> project);

		static void BuildCSharp(Ref<Project> project, bool restore = true);
		static void RestoreCSharp(Ref<Project> project);

		static Ref<Project> LoadFile(const std::filesystem::path& filepath);
		static void SaveFile(Ref<Project> project, const std::filesystem::path& filepath = std::filesystem::path());

		static Ref<Project> GenerateNew(const std::filesystem::path& path);

		static const std::string& GetProjectName()
		{
			return s_ActiveProject->GetConfig().Name;
		}

		static std::filesystem::path GetProjectDirectory()
		{
			return s_ActiveProject->GetConfig().ProjectDirectory;
		}

		template<typename T>
		static Ref<T> GetAsset(AssetHandle handle)
		{
			return std::static_pointer_cast<T>(Project::GetAssetManager()->GetAsset(handle));
		}

		static void CreateAsset(const std::filesystem::path& path)
		{
			Project::GetAssetManager()->CreateAsset(path);
		}

		static bool CanCreateAsset(AssetType type)
		{
			static Ref<Asset> nullAsset = nullptr;
			return Project::GetAssetManager()->CanCreateAsset(type, false, nullAsset);
		}

		static void RemoveAsset(AssetHandle handle)
		{
			Project::GetAssetManager()->RemoveAsset(handle);
		}

		static void SaveAsset(AssetHandle handle, const std::filesystem::path& path = std::filesystem::path())
		{
			Project::GetAssetManager()->SaveAsset(handle, path.empty() ? Project::GetFilePath(handle) : path);
		}

		static void ReloadAsset(AssetHandle handle)
		{
			Project::GetAssetManager()->ReloadAsset(handle);
		}

		static bool IsAssetLoaded(AssetHandle handle)
		{
			return Project::GetAssetManager()->IsAssetLoaded(handle);
		}

		static bool IsAssetLoaded(const std::string& name)
		{
			return Project::GetAssetManager()->IsAssetLoaded(name);
		}

		static bool IsAssetLoaded(const std::filesystem::path& filepath)
		{
			return Project::GetAssetManager()->IsAssetLoaded(filepath);
		}

		static AssetHandle ImportAsset(const std::filesystem::path& path)
		{
			return Project::GetAssetManager()->ImportAsset(path);
		}

		static AssetType GetAssetType(AssetHandle handle)
		{
			return Project::GetAssetManager()->GetAssetType(handle);
		}

		static const AssetHandle GetHandle(const std::string& name)
		{
			return Project::GetAssetManager()->GetHandle(name);
		}

		static const AssetHandle GetHandle(const std::filesystem::path& filepath)
		{
			return Project::GetAssetManager()->GetHandle(filepath);
		}

		static const std::filesystem::path& GetFilePath(AssetHandle handle)
		{
			return Project::GetAssetManager()->GetFilePath(handle);
		}

		static const std::string& GetName(AssetHandle handle)
		{
			return Project::GetAssetManager()->GetName(handle);
		}

		static void SaveAllAssets()
		{
			Project::GetAssetManager()->SaveAllAssets();
		}

	private:
		std::string GetMSbuildCommand(std::vector<const char*> commands);
	private:
		inline static Ref<Project> s_ActiveProject = nullptr;

		ProjectConfig m_Config;
		std::unordered_map<std::string, std::vector<InputBinding>> m_Bindings;

		Ref<AssetManager> m_AssetManager;
		Ref<ScriptEngine> m_ScriptEngine;
		Room* m_ActiveRoom;

		friend class ProjectSerializer;
		friend class ProjectSettingsPanel;
	};

}