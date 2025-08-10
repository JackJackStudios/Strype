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
			out << YAML::Key << "ViewportSize" << YAML::Value << m_Project->m_Config.ViewportSize;

			out << YAML::Key << "RuntimeProps" << YAML::BeginMap;
			{
				auto& props = m_Project->m_Config.RuntimeProps;
				out << YAML::Key << "Size" << YAML::Value << props.Size;
				out << YAML::Key << "Visible" << YAML::Value << props.Visible;
				out << YAML::Key << "Resizable" << YAML::Value << props.Resizable;
				out << YAML::Key << "Borderless" << YAML::Value << props.Borderless;
				out << YAML::Key << "VSync" << YAML::Value << props.VSync;
				out << YAML::Key << "WindowMode" << YAML::Value << magic_enum::enum_name(props.Mode);
			}
			out << YAML::EndMap;

			out << YAML::Key << "Bindings" << YAML::BeginMap;
			for (const auto& [name, bindings] : m_Project->m_Bindings)
			{
				out << YAML::Key << name << YAML::Flow;

				out << YAML::BeginSeq;
				{
					for (const auto& binding : bindings)
						out << binding;
				}
				out << YAML::EndSeq;
			}
			out << YAML::EndMap;
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
		config.ProjectDirectory = filepath.parent_path();

		config.StartRoom = root["StartRoom"].as<std::filesystem::path>();
		config.ViewportSize = root["ViewportSize"].as<glm::ivec2>();

		YAML::Node runtimeProps = root["RuntimeProps"];
		config.RuntimeProps.Size = runtimeProps["Size"].as<glm::ivec2>();
		config.RuntimeProps.Visible = runtimeProps["Visible"].as<bool>();
		config.RuntimeProps.Resizable = runtimeProps["Resizable"].as<bool>();
		config.RuntimeProps.Borderless = runtimeProps["Borderless"].as<bool>();
		config.RuntimeProps.VSync = runtimeProps["VSync"].as<bool>();
		config.RuntimeProps.Mode = magic_enum::enum_cast<AGI::WindowMode>(runtimeProps["WindowMode"].as<std::string_view>()).value_or(AGI::WindowMode::Windowed);

		YAML::Node bindings = root["Bindings"];
		for (auto it = bindings.begin(); it != bindings.end(); ++it)
		{
			std::string verb = it->first.as<std::string>();
			m_Project->m_Bindings[verb] = it->second.as<std::vector<InputBinding>>();
		}
	}

}