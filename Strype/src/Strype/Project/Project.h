#pragma once

#include "Strype/Asset/AssetManager.h"

namespace Strype {

	class ScriptEngine;
	class Room;

	struct ProjectConfig
	{
		//Serializated fields
		std::string Name;
		std::string StartRoom;

		//NOTE: Project directiory is folder .styproj is in to find assets etc.
		//		Working directiory is for editor-only assets like BaseShader
		std::filesystem::path ProjectDirectory;
		std::string ProjectFileName;
	};

	// Relative to working directory
	const std::filesystem::path EMPTY_PROJECT = "EmptyProject/EmptyProject.sproj";

	//Note: Project is not a Asset because it holds the AssetManager
	class Project
	{
	public:
		Project();
		~Project();

		const ProjectConfig& GetConfig() const { return m_Config; }

		static Ref<AssetManager> GetAssetManager() { return s_AssetManager; }
		static Ref<ScriptEngine>& GetScriptEngine() { return s_ScriptEngine; }
		static Room* GetActiveRoom() { return s_ActiveRoom; }

		static void SetActiveRoom(Room* room) { s_ActiveRoom = room; }

		static Ref<Project> GetActive() { return s_ActiveProject; }
		static void SetActive(Ref<Project> project);

		static void BuildProjectFiles(const std::filesystem::path& path);
		static void GenerateNew(const std::filesystem::path& path);

		static const std::string& GetProjectName()
		{
			return s_ActiveProject->GetConfig().Name;
		}

		static const std::string& GetStartRoom()
		{
			return s_ActiveProject->GetConfig().StartRoom;
		}

		static std::filesystem::path GetProjectDirectory()
		{
			return s_ActiveProject->GetConfig().ProjectDirectory;
		}

		template<typename T>
		static Ref<T> GetAsset(AssetHandle handle)
		{
			Ref<AssetManager> assetManager = Project::GetAssetManager();
			return std::static_pointer_cast<T>(assetManager->GetAsset(handle));
		}

		static void NewAsset(const std::filesystem::path& path)
		{
			Project::GetAssetManager()->NewAsset(path);
		}

		static void DeleteAsset(AssetHandle handle)
		{
			Project::GetAssetManager()->DeleteAsset(handle);
		}

		static void SaveAsset(Ref<Asset> asset, const std::filesystem::path& path)
		{
			Project::GetAssetManager()->GetSerializer(asset->GetType())->SaveAsset(asset, path);
		}

		static bool IsAssetLoaded(AssetHandle handle)
		{
			return Project::GetAssetManager()->IsAssetLoaded(handle);
		}

		static bool IsAssetLoaded(const std::filesystem::path& path)
		{
			return Project::GetAssetManager()->IsAssetLoaded(path);
		}

		static AssetHandle ImportAsset(const std::filesystem::path& path)
		{
			return Project::GetAssetManager()->ImportAsset(path);
		}

		static AssetType GetAssetType(AssetHandle handle)
		{
			return Project::GetAssetManager()->GetAssetType(handle);
		}

		static const AssetMetadata& GetMetadata(AssetHandle handle)
		{
			return Project::GetAssetManager()->GetMetadata(handle);
		}

		static const AssetHandle GetAssetHandle(const std::filesystem::path& path)
		{
			return Project::GetAssetManager()->GetHandle(path);
		}

		static const std::filesystem::path& GetFilePath(AssetHandle handle)
		{
			return Project::GetAssetManager()->GetFilePath(handle);
		}

		static void SaveAllAssets()
		{
			Project::GetAssetManager()->SaveAllAssets();
		}

		static void SaveAsset(AssetHandle handle, const std::filesystem::path& path = std::filesystem::path())
		{
			Project::GetAssetManager()->SaveAsset(handle, path.empty() ? Project::GetFilePath(handle) : path);
		}

	private:
		inline static Ref<AssetManager> s_AssetManager;
		inline static Ref<ScriptEngine> s_ScriptEngine;
		inline static Room* s_ActiveRoom;

		ProjectConfig m_Config;
		inline static Ref<Project> s_ActiveProject;

		friend class ProjectSerializer;
	};

}