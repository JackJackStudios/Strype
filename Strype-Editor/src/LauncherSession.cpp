#include "LauncherSession.hpp"
#include "EditorSession.hpp"

#include <Strype.hpp>
#include <yaml-cpp/yaml.h>

namespace Strype {

    bool LauncherSession::LoadConfig(const std::filesystem::path& filepath)
    {
        if (!std::filesystem::exists(filepath)) return false;
        YAML::Node root = YAML::LoadFile(filepath.string())["Launcher"];

        for (const auto& project : root["Recent Projects"])
        {
            if (!std::filesystem::exists(project.as<std::string>())) continue;
            m_LoadedProjects.emplace_back(Project::LoadFile(project.as<std::string>()));
        }

        return true;
    }

    void LauncherSession::SaveConfig(const std::filesystem::path& filepath)
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

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

	void LauncherSession::OnImGuiRender()
	{
        ImVec4 colour = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
        Render->SetClearColour({ colour.x, colour.y, colour.z });
        Render->Clear();

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        float padding = 5.0f;

        ImGui::SetNextWindowPos({ viewport->Pos.x + padding, viewport->Pos.y + padding });
        ImGui::SetNextWindowSize({ viewport->Size.x - padding * 2, viewport->Size.y - padding * 2 });
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f)); // Inner content padding
        ImGui::Begin("DockSpace", 0, window_flags);
        ImGui::PopStyleVar(3);

        if (ImGui::Button("+ Create")) 
        {
            if (auto filepath = FileDialogs::OpenFolder(); !filepath.empty())
            {
                Ref<Project> project = Project::GenerateNew(filepath);
                m_LoadedProjects.emplace_back(project);
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Import")) 
        {
            if (auto filepath = FileDialogs::OpenFile("Strype Project (.sproj)\0*.sproj\0"); !filepath.empty())
            {
                Ref<Project> project = Project::LoadFile(filepath);
                m_LoadedProjects.emplace_back(project);
            }
        }

        ImGui::SameLine();

        char filter[128] = "";
        ImGui::SetNextItemWidth(200);
        ImGui::InputTextWithHint("##filter", "Filter Projects", filter, IM_ARRAYSIZE(filter));

        ImGui::Separator();

        std::string filterStr = filter;
        std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), ::tolower);

        for (auto& project : m_LoadedProjects)
        {
            ImGui::PushID(project.get());
            const std::string& name = project->GetConfig().Name;

            // Convert project name to lowercase
            std::string lowerName = name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

            // Skip projects that don't matc\h filter
            if (!filterStr.empty() && lowerName.find(filterStr) == std::string::npos)
                continue;

            float iconSize = 40.0f;
            float fullWidth = ImGui::GetContentRegionAvail().x;
            ImVec2 cursor = ImGui::GetCursorPos();

            if (ImGui::Selectable("", false, 0, ImVec2(fullWidth, iconSize)))
            {
                Application::Get().NewSession<EditorSession>(project);
                Application::Get().DispatchEvent<WindowCloseEvent>();
            }

            ImGui::SetCursorPos(cursor);
            ProjectWidget(project, iconSize);

            ImGui::PopID();
        }

        ImGui::End();
	}

    void LauncherSession::ProjectWidget(Ref<Project> project, float iconSize)
    {
        ImGui::BeginGroup();

        ImGui::Image(m_ProjectIcon->GetRendererID(), ImVec2(iconSize, iconSize));
        ImGui::SameLine();

        ImGui::BeginGroup();

        ImGui::Text(project->GetConfig().Name.c_str());
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.f), project->GetConfig().ProjectDirectory.string().c_str());

        ImGui::EndGroup();

        ImGui::EndGroup();
    }

};