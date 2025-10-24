#include <Strype.hpp>
#include "EditorSession.hpp"

#include "Strype/Utils/ScopedDockspace.hpp"
#include <ImGuizmo.h>

namespace Strype {
	
	void EditorSession::OnAttach()
	{
		//m_Project->RestoreCSharp();
		Project::SetActive(m_Project);
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

		for (int i = 0; i < m_EditorPanels.size(); ++i)
		{
			auto& panel = m_EditorPanels[i];

			if (panel->m_IsOpen)
			{
				ImGui::Begin(panel->Title.c_str(), panel->Closing ? &panel->m_IsOpen : nullptr, panel->Flags);
				panel->OnImGuiRender();
				ImGui::End();
			}
			else
			{
				m_EditorPanels.erase(m_EditorPanels.begin() + i);
			}
		}
	}

	void EditorSession::OnEvent(Event& e)
	{
		for (const auto& panel : m_EditorPanels)
			panel->OnEvent(e);
	}

};