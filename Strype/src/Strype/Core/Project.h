#pragma once

namespace Strype {

	struct ProjectConfig
	{
		std::string Name;

		std::string AssetDirectory = "assets";

		//Note: Project directiory is folder .styproj is in to find assets etc.
		//		Working directiory is for editor-only assets like BaseShader
		std::string ProjectDirectory;
	};

	class Project
	{
	public:
		Project();
		~Project();

		const ProjectConfig& GetConfig() const { return m_Config; }

		static Ref<Project> GetActive() { return s_ActiveProject; }
		static void SetActive(Ref<Project> project);

		static const std::string& GetProjectName()
		{
			return s_ActiveProject->GetConfig().Name;
		}

		static std::filesystem::path GetProjectDirectory()
		{
			return s_ActiveProject->GetConfig().ProjectDirectory;
		}

		std::filesystem::path GetAssetDirectory() const
		{
			return std::filesystem::path(GetConfig().ProjectDirectory) / GetConfig().AssetDirectory;
		}
	private:
		ProjectConfig m_Config;
		inline static Ref<Project> s_ActiveProject;
	};

}