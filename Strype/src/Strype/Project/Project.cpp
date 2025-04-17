#include "stypch.h"
#include "Project.h"

#include "Strype/Project/ProjectSerializer.h"
#include "Strype/Script/ScriptEngine.h"

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
					STY_CORE_ERROR("Could not read from file '{0}'", filepath.string());
				}
			}
			else
			{
				STY_CORE_ERROR("Could not open file '{0}'", filepath.string());
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

	void Project::GenerateNew(const std::filesystem::path& path)
	{
		// Copy empty project
		std::filesystem::path templateDir = EMPTY_PROJECT.parent_path();

		for (const auto& entry : std::filesystem::recursive_directory_iterator(templateDir)) {
			const auto& entryPath = entry.path();
			auto relativePath = std::filesystem::relative(entryPath, templateDir);
			auto destPath = path / relativePath;

			if (std::filesystem::is_directory(entryPath)) {
				std::filesystem::create_directories(destPath);
			}
			else {
				std::filesystem::copy_file(entryPath, destPath, std::filesystem::copy_options::overwrite_existing);
			}
		}

		// Copy premake5.lua file to new project
		std::string content = Utils::ReadFile("assets/defaults/premake5.lua");
		Utils::ReplaceKeyWord(content, "{0}", path.filename().string());

		std::ofstream out((path / "premake5.lua").string(), std::ios::out | std::ios::binary);
		out << content;
		out.close();

		// Build C# project
		std::filesystem::path workingdir = std::filesystem::current_path();

		std::filesystem::current_path(path);
		system("%STRYPE_DIR%/Libraries/premake/premake5.exe --verbose vs2022 >nul 2>&1");
		std::filesystem::current_path(workingdir);

		std::filesystem::remove(path / "premake5.lua");

		//Change empty project to fit new project name
		std::string projName = path.filename().string();
		std::filesystem::rename(path / EMPTY_PROJECT.filename(), path / (path.filename().string() + ".sproj"));
	}

	void Project::SetActive(Ref<Project> project)
	{
		if (s_ActiveProject)
		{
			s_AssetManager->SaveAllAssets();

			s_ScriptEngine.reset();
			s_AssetManager.reset();
			s_ActiveRoom.reset();
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