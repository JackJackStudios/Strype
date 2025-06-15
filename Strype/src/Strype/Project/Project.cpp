#include "stypch.hpp"
#include "Project.hpp"

#include "Strype/Project/ProjectSerializer.hpp"
#include "Strype/Script/ScriptEngine.hpp"

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

		static std::string ReadFile(const std::filesystem::path& filepath)
		{
			std::string result;
			std::ifstream in(filepath, std::ios::in | std::ios::binary);
			if (in)
			{
				in.seekg(0, std::ios::end);
				size_t size = in.tellg();
				if (size != -1)
				{
					result.resize(size);
					in.seekg(0, std::ios::beg);
					in.read(&result[0], size);
					in.close();
				}
				else
				{
					STY_CORE_ERROR("Could not read from file '{0}'", filepath);
				}
			}
			else
			{
				STY_CORE_ERROR("Could not open file '{0}'", filepath);
			}

			return result;
		}

	}

	Project::Project()
	{
	}

	Project::~Project()
	{
	}

	void Project::BuildProjectFiles(const std::filesystem::path& path)
	{
		std::filesystem::path workingdir = std::filesystem::current_path();
		std::string content = Utils::ReadFile("assets/premake5.lua");
		Utils::ReplaceKeyWord(content, std::filesystem::path(EMPTY_PROJECT).stem().string(), path.stem().string());

		std::ofstream out((path / HIDDEN_FOLDER / "premake5.lua").string(), std::ios::out | std::ios::binary);
		out << content;
		out.close();

		// Build C# project
		std::filesystem::current_path(path / HIDDEN_FOLDER);
		system("%STRYPE_DIR%/Strype/master/premake5.exe --verbose vs2022 > nul 2>&1");

		std::filesystem::current_path(workingdir);
	}

	void Project::GenerateNew(const std::filesystem::path& path)
	{
		// Create nessacry folders
		std::filesystem::create_directories(path / HIDDEN_FOLDER / "ScriptCore");

		// Copy empty project
		std::filesystem::path templateDir = std::filesystem::path(EMPTY_PROJECT).parent_path();
		Utils::CopyDirectory(templateDir, path);
		Utils::CopyDirectory("assets/ScriptCore", path / HIDDEN_FOLDER / "ScriptCore");

		BuildProjectFiles(path);

		//Change empty project to fit new project name
		std::filesystem::rename(path / std::filesystem::path(EMPTY_PROJECT).filename(), path / (path.filename().string() + ".sproj"));
	}

	void Project::SetActive(Ref<Project> project)
	{
		if (s_ActiveProject)
		{
			s_ScriptEngine.reset();
			s_AssetManager.reset();
			s_ActiveRoom = nullptr;
		}

		s_ActiveProject = project;
		if (s_ActiveProject)
		{
			Ref<ScriptEngine> scriptEngine = CreateRef<ScriptEngine>(project);
			s_ScriptEngine = scriptEngine;

			Ref<AssetManager> assetManager = CreateRef<AssetManager>();
			s_AssetManager = assetManager;
			s_AssetManager->ReloadAssets(); // <- This must not happen in constructor
		}
	}

}