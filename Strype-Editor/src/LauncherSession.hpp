#pragma once

#include <Strype.hpp>
#include <yaml-cpp/yaml.h>

#include "Strype/Utils/YamlHelpers.hpp"

namespace Strype {

	const std::filesystem::path LAUNCHER_CONFIG = "assets/Launcher.yaml";

	class LauncherSession : public Session
	{
	public:
		LauncherSession()
		{
			WindowProps.Title = "Strype Launcher";
			WindowProps.Size = { 853, 480 };
			WindowProps.Resizable = false;

			ImGuiEnabled = true;
			DockspaceEnabled = false;
		}

		void OnAttach() override
		{
			m_ProjectIcon = Utils::LoadTexture("assets/icons/ProjectIcon.png");

			if (!std::filesystem::exists(LAUNCHER_CONFIG)) return;
			YAML::Node root = YAML::LoadFile(LAUNCHER_CONFIG.string())["Launcher"];

			for (const auto& project : root["Recent Projects"])
			{
				std::filesystem::path filepath = project.as<std::filesystem::path>();
				if (!std::filesystem::exists(filepath))
				{
					STY_LOG_ERROR("Launcher", "File not found: {}", filepath);
					continue;
				}

				m_LoadedProjects.push_back(Project::LoadFile(filepath));
			}
		}

		void OnDetach() override
		{
			YAML::Emitter out;
			out << YAML::BeginMap;
			out << YAML::Key << "Launcher" << YAML::Value << YAML::BeginMap;
			{
				out << YAML::Key << "Recent Projects" << YAML::BeginSeq;
				for (const auto& project : m_LoadedProjects)
				{
					out << (project->GetConfig().ProjectDirectory / (project->GetConfig().Name + ".sproj")).string();
				}

				out << YAML::EndSeq;
			}
			out << YAML::EndMap;
			out << YAML::EndMap;

			Utils::WriteFile(LAUNCHER_CONFIG, out.c_str());
		}

		void OnImGuiRender() override;

	private:
		std::vector<Ref<Project>> m_LoadedProjects;
		AGI::Texture m_ProjectIcon;
	};

};