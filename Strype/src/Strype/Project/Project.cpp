#include "stypch.hpp"
#include "Project.hpp"

#include "Strype/Utils/PlatformUtils.hpp"
#include "Strype/Script/ScriptEngine.hpp"

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

	}

	std::string Project::GetMSbuildCommand(std::vector<const char*> commands)
	{
#ifdef STY_WINDOWS
		auto& path = m_Config.ProjectDirectory;

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

	void Project::BuildCSharp(Ref<Project> project, bool restore)
	{
		STY_CORE_TRACE("Building C# project '{}'", project->GetConfig().ProjectDirectory);

		if (restore) RestoreCSharp(project);
		std::filesystem::remove_all(project->GetConfig().ProjectDirectory / Project::HiddenFolder / "bin");

		std::string command = project->GetMSbuildCommand({ "build" });
		system(command.c_str());
	}

	void Project::RestoreCSharp(Ref<Project> project)
	{
		auto& path = project->GetConfig().ProjectDirectory;

		std::string content = Utils::ReadFile("assets/premake5.lua");
		Utils::ReplaceKeyWord(content, Project::EmptyProject.stem().string(), path.stem().string());

		Utils::WriteFile(path / Project::HiddenFolder / "premake5.lua", content);

#ifdef STY_WINDOWS
		system(fmt::format("cd \"{}\" && %STRYPE_DIR%/Strype/master/premake5.exe --verbose vs2022 > nul 2>&1", (path / Project::HiddenFolder).string()).c_str());
		system(project->GetMSbuildCommand({ "restore" }).c_str());
#endif
	}

	Ref<Project> Project::GenerateNew(const std::filesystem::path& path)
	{
		// Create nessacry folders
		std::filesystem::create_directories(path / Project::HiddenFolder);

		// Copy empty project
		Utils::CopyDirectory(Project::EmptyProject.parent_path(), path);

		//Change empty project to fit new project name
		std::filesystem::rename(path / Project::EmptyProject.filename(), path / (path.filename().string() + ".sproj"));

		Ref<Project> project = Project::LoadFile(path / (path.filename().string() + ".sproj"));
		Project::BuildCSharp(project);

		return project;
	}

	void Project::SetActive(Ref<Project> project)
	{
		if (s_ActiveProject)
		{
			s_ActiveProject->m_ActiveRoom = nullptr;
			s_ActiveProject->m_ScriptEngine->UnloadAssembly();

			s_ActiveProject->m_AssetManager.reset();
		}

		s_ActiveProject = project;
		if (s_ActiveProject)
		{
			Ref<ScriptEngine> scriptEngine = CreateRef<ScriptEngine>(project);
			project->m_ScriptEngine = scriptEngine;

			Ref<AssetManager> assetManager = CreateRef<AssetManager>();
			project->m_AssetManager = assetManager;
			project->m_AssetManager->LoadAllAssets(project); // <- This must not happen in constructor
		}
	}

}