#include "stypch.hpp"
#include "ProjectSerializer.hpp"

#include "Strype/Utils/YamlHelpers.hpp"

namespace Strype {

	ProjectSerializer::ProjectSerializer(const Ref<Project>& project)
		: m_Project(project)
	{
	}

	void ProjectSerializer::Serialize(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Project" << YAML::Value;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "StartRoom" << YAML::Value << m_Project->m_Config.StartRoom;
		}
		out << YAML::EndMap;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void ProjectSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		YAML::Node root = YAML::LoadFile(filepath.string())["Project"];
		STY_CORE_VERIFY(root, "Could not open project");

		STY_CORE_INFO("Loading project '{}'", filepath.stem());

		ProjectConfig& config = m_Project->m_Config;
		config.Name = filepath.stem().string();
		config.StartRoom = root["StartRoom"].as<std::filesystem::path>();
		config.ProjectDirectory = filepath.parent_path();
	}

}