#include "PanelManager.hpp"

namespace Strype {

	PanelManager::PanelManager()
	{
		m_Inspector = AddPanel<InspectorPanel>();

		for (const Ref<EditorPanel>& panel : m_Panels)
			panel->m_Inspector = m_Inspector;
	}

	PanelManager::~PanelManager()
	{
		m_Panels.clear();
	}

	void PanelManager::OnUpdate(float ts)
	{
		for (const Ref<EditorPanel>& panel : m_Panels)
			panel->OnUpdate(ts);
	}

	void PanelManager::OnImGuiRender()
	{
		for (int i=0; i<m_Panels.size(); ++i)
		{
			Ref<EditorPanel> panel = m_Panels[i];
			if (panel->m_IsOpen)
			{
				if (panel->Closing)
					ImGui::Begin(panel->Title.c_str(), &panel->m_IsOpen, panel->Flags);
				else
					ImGui::Begin(panel->Title.c_str(), 0, panel->Flags);

				panel->OnImGuiRender();
				ImGui::End();
			}
			else
			{
				m_Panels.erase(m_Panels.begin() + i);
			}
		}
	}

	void PanelManager::OnEvent(Event& e)
	{
		for (const Ref<EditorPanel>& panel : m_Panels)
			panel->OnEvent(e);
	}

	void PanelManager::SetRoomContext(const Ref<Room>& context)
	{
		for (const Ref<EditorPanel>& panel : m_Panels)
			panel->m_CurrentRoom = context;

		m_ActiveRoom = context;
	}

	void PanelManager::OnProjectChanged()
	{
		for (const Ref<EditorPanel>& panel : m_Panels)
			panel->OnProjectChanged();
	}

}