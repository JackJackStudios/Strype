#include "stypch.hpp"
#include "Project.hpp"

#include "Strype/Utils/PlatformUtils.hpp"

#include "Strype/Project/ProjectSerializer.hpp"
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

	Project::Project()
	{
	}

	Project::~Project()
	{
	}

	// Windows only
	void Project::BuildCSharp(Ref<Project> project, bool restore)
	{
		if (restore) RestoreCSharp(project);
		auto& path = project->GetConfig().ProjectDirectory;

#ifdef STY_WINDOWS
		STY_CORE_INFO("Building C# project '{}'", path);

		system(std::format("cd \"{}\" && premake5.exe --verbose vs2022", (path / HIDDEN_FOLDER).string()).c_str());
		std::filesystem::remove_all(path / HIDDEN_FOLDER / "bin");

		TCHAR programFilesFilePath[MAX_PATH];
		SHGetSpecialFolderPath(0, programFilesFilePath, CSIDL_PROGRAM_FILES, FALSE);
		std::filesystem::path msBuildPath = std::filesystem::path(programFilesFilePath) / "Microsoft Visual Studio" / "2022" / "Community" / "Msbuild" / "Current" / "Bin" / "MSBuild.exe";
		std::string command = std::format("cd \"{}\" && \"{}\" \"{}.sln\" -property:Configuration={} -t:restore,build", path.string(), msBuildPath.string(), (HIDDEN_FOLDER / path.filename()).string(), STY_BUILD_CONFIG_NAME);

		system(command.c_str());
#endif
	}

	void Project::RestoreCSharp(Ref<Project> project)
	{
		auto& path = project->GetConfig().ProjectDirectory;

		std::string content = Utils::ReadFile("assets/premake5.lua");
		Utils::ReplaceKeyWord(content, std::filesystem::path(EMPTY_PROJECT).stem().string(), path.stem().string());

		Utils::WriteFile(path / HIDDEN_FOLDER / "premake5.lua", content);

		// Build C# project
		system(std::format("cd \"{}\" && %STRYPE_DIR%/Strype/master/premake5.exe --verbose vs2022 > nul 2>&1", (path / HIDDEN_FOLDER).string()).c_str());
	}

	Ref<Project> Project::GenerateNew(const std::filesystem::path& path)
	{
		// Create nessacry folders
		std::filesystem::create_directories(path / HIDDEN_FOLDER);

		// Copy empty project
		std::filesystem::path templateDir = std::filesystem::path(EMPTY_PROJECT).parent_path();
		Utils::CopyDirectory(templateDir, path);

		//Change empty project to fit new project name
		std::filesystem::rename(path / std::filesystem::path(EMPTY_PROJECT).filename(), path / (path.filename().string() + ".sproj"));

		Ref<Project> project = CreateRef<Project>();
		ProjectSerializer serializer(project);
		serializer.Deserialize(path / (path.filename().string() + ".sproj"));

		Project::BuildCSharp(project);
		return project;
	}

	void Project::SetActive(Ref<Project> project)
	{
		if (s_ActiveProject)
		{
			s_ActiveProject->m_ScriptEngine.reset();
			s_ActiveProject->m_AssetManager.reset();
			s_ActiveProject->m_ActiveRoom = nullptr;
		}

		s_ActiveProject = project;
		if (s_ActiveProject)
		{
			Ref<ScriptEngine> scriptEngine = CreateRef<ScriptEngine>(project);
			project->m_ScriptEngine = scriptEngine;

			Ref<AssetManager> assetManager = CreateRef<AssetManager>();
			project->m_AssetManager = assetManager;
			project->m_AssetManager->ReloadAssets(); // <- This must not happen in constructor
		}
	}

}