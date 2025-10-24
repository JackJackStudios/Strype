#include "stypch.hpp"
#include "Project.hpp"

#include "Strype/Utils/PlatformUtils.hpp"
#include "Strype/Script/ScriptEngine.hpp"
#include "Strype/Core/Application.hpp"

#ifdef STY_WINDOWS
#	include <ShlObj.h>
#endif

namespace Strype {

	namespace Utils {

		static void ReplaceKeyWord(std::string& str, const std::string& keyword, const std::string& replace)
		{
			size_t pos = 0;
			while ((pos = str.find(keyword, pos)) != std::string::npos)
			{
				str.replace(pos, keyword.length(), replace);
				pos += replace.length();
			}
		}

		static void CopyDirectory(const std::filesystem::path& srcpath, const std::filesystem::path& destcpath)
		{
			for (const auto& entry : std::filesystem::recursive_directory_iterator(srcpath)) 
			{
				const auto& entryPath = entry.path();
				auto relativePath = std::filesystem::relative(entryPath, srcpath);
				auto destPath = destcpath / relativePath;

				if (std::filesystem::is_directory(entryPath)) 
				{
					std::filesystem::create_directories(destPath);
				}
				else 
				{
					std::filesystem::copy_file(entryPath, destPath, std::filesystem::copy_options::overwrite_existing);
				}
			}
		}

		std::string GetMSbuildCommand(Project* project, std::vector<const char*> commands)
		{
#ifdef STY_WINDOWS
			auto& path = project->GetConfig().ProjectDirectory;

			TCHAR programFilesFilePath[MAX_PATH];
			SHGetSpecialFolderPath(0, programFilesFilePath, CSIDL_PROGRAM_FILES, FALSE);
			std::filesystem::path msBuildPath = std::filesystem::path(programFilesFilePath) / "Microsoft Visual Studio" / "2022" / "Community" / "Msbuild" / "Current" / "Bin" / "MSBuild.exe";
			std::string command = fmt::format("cd \"{}\" && \"{}\" \"{}.sln\" /nologo /clp:ErrorsOnly;WarningsOnly /m /nr:true /verbosity:minimal /p:Configuration={} -t:", path.string(), msBuildPath.string(), Project::HiddenFolder / path.filename(), STY_BUILD_CONFIG_NAME);

			for (int i = 0; i < commands.size(); ++i)
			{
				command.append(commands[i]);

				if (i != commands.size() - 1)
					command.append(",");
			}

			return command;
#endif
			return "";
		}

	}

	void Project::BuildCSharp(bool restore)
	{
		STY_LOG_TRACE("Project", "Building C# project '{}'", m_Config.ProjectDirectory);
		if (restore) RestoreCSharp();

#ifdef STY_WINDOWS
		std::filesystem::remove_all(m_Config.ProjectDirectory / Project::HiddenFolder / "bin");
		system(Utils::GetMSbuildCommand(this, { "build" }).c_str());
#endif
	}

	void Project::RestoreCSharp()
	{
		std::string content = Utils::ReadFile("assets/premake5.lua");

		auto& path = m_Config.ProjectDirectory;
		Utils::ReplaceKeyWord(content, Project::EmptyProject.stem().string(), path.stem().string());

		Utils::WriteFile(path / Project::HiddenFolder / "premake5.lua", content);

#ifdef STY_WINDOWS
		system(fmt::format("cd \"{}\" && %STRYPE_DIR%/Strype/master/premake5.exe --verbose vs2022 > nul 2>&1", (path / Project::HiddenFolder).string()).c_str());
		system(Utils::GetMSbuildCommand(this, { "restore" }).c_str());
#endif
	}

	Ref<Project> Project::GenerateNew(const std::filesystem::path& path)
	{
		std::filesystem::create_directories(path / Project::HiddenFolder);
		Utils::CopyDirectory(Project::EmptyProject.parent_path(), path);
		
		std::filesystem::path newProject = path / (path.filename().string() + ".sproj");

		std::filesystem::rename(path / Project::EmptyProject.filename(), newProject);
		Ref<Project> project = Project::LoadFile(newProject);

		project->BuildCSharp();
		return project;
	}

	void Project::SetActive(Ref<Project> project)
	{
		if (project == s_ActiveProject) return;

		if (s_ActiveProject)
		{
			s_ActiveProject->m_ActiveRoom = nullptr;
			s_ActiveProject->m_ScriptEngine->UnloadAssembly();
		}

		s_ActiveProject = project;
		if (s_ActiveProject)
		{
			Timer projectTime;
			STY_LOG_INFO("Project", "Loading project \"{}\" ", s_ActiveProject->GetConfig().Name);

			project->m_ScriptEngine = CreateRef<ScriptEngine>(project);
			project->m_AssetManager = CreateRef<AssetManager>();

			project->m_AssetManager->LoadAllAssets(project); // <- This must not happen in constructor

			STY_LOG_TRACE("Project", "Loaded \"{}\" in {} seconds", s_ActiveProject->GetConfig().Name, projectTime.ElapsedSeconds());
		}

		Application::Get().DispatchEvent<ProjectChangedEvent>(s_ActiveProject);
	}

}