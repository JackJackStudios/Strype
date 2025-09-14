#pragma once

#include "FileWatch.hpp"
#include "Panels/PanelManager.hpp"
#include "Panels/ContentBrowserPanel.hpp"
#include "Panels/ProjectSettingsPanel.hpp"
#include "Panels/RoomPanel.hpp"

#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Strype {

	static std::string ToLower(const std::string& s)
	{
		std::string result = s;

		std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
		return result;
	}

	class EditorSession : public Session
	{
	public:
		~EditorSession();
		EditorSession(Ref<Project> project)
			: m_Project(project)
		{
			WindowProps.Title = "Strype-Editor";
			WindowProps.Mode = AGI::WindowMode::Maximized;
			
			ImGuiEnabled = true;
			DockspaceEnabled = true;
		}

		void OnAttach() override;
		void OnUpdate(float ts) override;
		void OnImGuiRender() override;

		void OnEvent(Event& e) override;

		void OpenRoom(AssetHandle handle);

		void NewProject(const std::filesystem::path& path = std::filesystem::path());
		void SaveProject();

		void OpenProject(const std::filesystem::path& path = std::filesystem::path());
		void OpenProject(Ref<Project> project);
	private:
		void OnWindowDrop(WindowDropEvent& e);

		void UI_RoomPanel();
		void OnFilewatcher(const std::filesystem::path& filepath, const filewatch::Event event);
	private:
		Ref<filewatch::FileWatch<std::string>> m_FileWatcher;

		PanelManager m_PanelManager;
		Ref<ContentBrowserPanel> m_ContentBrowserPanel;

		Ref<Project> m_Project;
	};

}