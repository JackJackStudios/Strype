#include "stypch.hpp"
#include "Project.hpp"

#include "Strype/Utils/YamlHelpers.hpp"

namespace Strype {

	void Project::SaveAll(Ref<Project> project, const std::filesystem::path& filepath)
	{
		project->m_AssetManager->SaveAllAssets();

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Project" << YAML::Value;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "StartRoom" << YAML::Value << project->m_Config.StartRoom;
			out << YAML::Key << "ViewportSize" << YAML::Value << project->m_Config.ViewportSize;

			out << YAML::Key << "RuntimeProps" << YAML::BeginMap;
			{
				auto& props = project->m_Config.RuntimeProps;
				out << YAML::Key << "Size" << YAML::Value << props.Size;
				out << YAML::Key << "Visible" << YAML::Value << props.Visible;
				out << YAML::Key << "Resizable" << YAML::Value << props.Resizable;
				out << YAML::Key << "Borderless" << YAML::Value << props.Borderless;
				out << YAML::Key << "VSync" << YAML::Value << props.VSync;
				out << YAML::Key << "WindowMode" << YAML::Value << magic_enum::enum_name(props.Mode);
			}
			out << YAML::EndMap;

			out << YAML::Key << "Bindings" << YAML::BeginMap;
			for (const auto& [name, bindings] : project->m_Bindings)
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

		std::ofstream fout(filepath.empty() ? project->m_Config.ProjectDirectory / (project->m_Config.Name + ".sproj") : filepath);
		fout << out.c_str();
	}

	Ref<Project> Project::LoadFile(const std::filesystem::path& filepath)
	{
		Ref<Project> project = CreateRef<Project>();

		YAML::Node root = YAML::LoadFile(filepath.string())["Project"];
		if (!root) return nullptr;

		STY_CORE_INFO("Loading project '{}'", filepath.stem());

		ProjectConfig& config = project->m_Config;
		config.Name = filepath.stem().string();
		config.ProjectDirectory = filepath.parent_path();

		config.StartRoom = root["StartRoom"].as<std::string>();
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
			project->m_Bindings[verb] = it->second.as<std::vector<InputBinding>>();
		}

		return project;
	}

}