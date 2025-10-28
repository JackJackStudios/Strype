#include <Strype.hpp>
#include "EditorSession.hpp"

#include "Strype/Utils/ScopedDockspace.hpp"
#include <ImGuizmo.h>

#include "Panels/RoomPanel.hpp"

namespace Strype {
	
	class ObjectsPanel : public EditorPanel
	{
	public:
		ObjectsPanel()
		{
			Title = "Objects";
			Closing = false;
		}
	};

	class InspectorPanel : public EditorPanel
	{
	public:
		InspectorPanel()
		{
			Title = "Inspector";
			Closing = false;
		}
	};

	class AssetsPanel : public EditorPanel
	{
	public:
		AssetsPanel()
		{
			Title = "Assets";
			Closing = false;
		}
	};

	void EditorSession::OnAttach()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("assets/editor-font.ttf", 16);

		//m_Project->RestoreCSharp();
		Project::SetActive(m_Project);
		m_ActiveRoom = Project::GetAsset<Room>(Project::GetAssetManager()->GetHandle(m_Project->GetConfig().StartRoom));

		AddPanel<RoomPanel>();
	}

	void EditorSession::OnDetach()
	{
		Project::SaveAll(m_Project);
	}

	void EditorSession::OnUpdate(float ts)
	{
		for (const auto& panel : m_EditorPanels)
			panel->OnUpdate(ts);
	}

	void EditorSession::OnImGuiRender()
	{
		ImGuizmo::BeginFrame();
		ScopedDockspace dockspace;

		for (auto it = m_EditorPanels.begin(); it != m_EditorPanels.end(); )
		{
			auto& panel = *it;

			if (!panel->m_IsOpen)
			{
				it = m_EditorPanels.erase(it);
				continue;
			}

			if (!panel->ManualControl)
				ImGui::Begin(panel->Title.c_str(), panel->Closing ? &panel->m_IsOpen : nullptr, panel->Flags);

			panel->OnImGuiRender();

			if (!panel->ManualControl)
				ImGui::End();

			++it;
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Project"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	void EditorSession::OnEvent(Event& e)
	{
		if (e.GetEventType() == EventType::WindowClose)
			// Closes runtime as well
			Application::Get().DispatchEvent<ApplicationQuitEvent>();

		for (const auto& panel : m_EditorPanels)
			panel->OnEvent(e);
	}

};