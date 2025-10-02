#pragma once

#include "Strype/Core/Input.hpp"

#include "Strype/Project/Asset.hpp"
#include "Strype/Project/AssetManager.hpp"

#include <AGI/agi.hpp>

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
	
	// NOTE: Project is not a Asset because it holds the AssetManager
	class Project
	{
	public:
		inline static const std::filesystem::path EmptyProject = "assets/EmptyProject/EmptyProject.sproj";
		inline static const std::filesystem::path HiddenFolder = "strype";

		const ProjectConfig& GetConfig() const { return m_Config; }

		static Ref<AssetManager> GetAssetManager() { return GetActive()->m_AssetManager; }
		static Ref<ScriptEngine> GetScriptEngine() { return GetActive()->m_ScriptEngine; }
		static Room* GetActiveRoom() { return GetActive()->m_ActiveRoom; }

		static void SetActiveRoom(Room* room) { GetActive()->m_ActiveRoom = room; }

		static Ref<Project> GetActive() { return s_ActiveProject; }
		static void SetActive(Ref<Project> project);

		static void BuildCSharp(Ref<Project> project, bool restore = true);
		static void RestoreCSharp(Ref<Project> project);

		static Ref<Project> LoadFile(const std::filesystem::path& filepath);
		static void SaveAll(Ref<Project> project, const std::filesystem::path& filepath = std::filesystem::path());

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
			return std::static_pointer_cast<T>(GetAssetManager()->GetAsset(handle));
		}

		bool VerbExists(const std::string& str)
		{
			return m_Bindings.find(str) == m_Bindings.end();
		}

		const std::vector<InputBinding>& GetBindings(const std::string& str)
		{
			return m_Bindings[str];
		}
	private:
		std::string GetMSbuildCommand(std::vector<const char*> commands);
	private:
		inline static Ref<Project> s_ActiveProject = nullptr;

		ProjectConfig m_Config;
		std::unordered_map<std::string, std::vector<InputBinding>> m_Bindings;

		Ref<AssetManager> m_AssetManager;
		Ref<ScriptEngine> m_ScriptEngine;
		Room* m_ActiveRoom = nullptr;
		 
		friend class ProjectSerializer;
		friend class ProjectSettingsPanel;
	};

}