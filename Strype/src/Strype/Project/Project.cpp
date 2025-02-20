#include "stypch.h"
#include "Project.h"

#include "Strype/Project/ProjectSerializer.h"

namespace Strype {

	Project::Project()
	{
	}

	Project::~Project()
	{
	}

	Ref<Project> Project::New(const std::filesystem::path& path)
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

		//Change empty project to fit new project name
		std::string projName = path.filename().string();
		std::filesystem::rename(path / EMPTY_PROJECT.filename(), path / (path.filename().string() + ".sproj"));

		// Load files into memory
		Ref<Project> project = CreateRef<Project>();
		ProjectSerializer serializer(project);
		serializer.Deserialize(path / (projName + ".sproj"));

		return project;
	}

	void Project::SetActive(Ref<Project> project)
	{
		if (s_ActiveProject)
		{
		}

		s_ActiveProject = project;

		Ref<AssetManager> assetManager = CreateRef<AssetManager>();
		s_AssetManager = assetManager;
	}

}