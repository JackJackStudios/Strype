#include "stypch.h"
#include "ProjectSerializer.h"

#include "Strype/Utils/YamlHelpers.h"

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
			out << YAML::Key << "Name" << YAML::Value << m_Project->m_Config.Name;
			out << YAML::Key << "StartRoom" << YAML::Value << m_Project->m_Config.StartRoom;
		}
		out << YAML::EndMap;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void ProjectSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		std::ifstream fstream(filepath);

		STY_CORE_ASSERT(fstream.is_open(), "Error opening file");

		std::stringstream stream;
		stream << fstream.rdbuf();

		YAML::Node data = YAML::Load(stream.str());
		YAML::Node root = data["Project"];

		STY_CORE_ASSERT(root, "Could not open project");
		STY_CORE_ASSERT(root["Name"], "Could not open project");

		STY_CORE_TRACE("Deserializing project '{0}'", root["Name"].as<std::string>());

		ProjectConfig& config = m_Project->m_Config;
		config.Name = root["Name"].as<std::string>();
		config.StartRoom = root["StartRoom"].as<std::string>();

		std::filesystem::path projectPath = filepath;
		config.ProjectFileName = projectPath.filename().string();
		config.ProjectDirectory = projectPath.parent_path().string();
	}

}