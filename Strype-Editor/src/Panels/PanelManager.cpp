#include "PanelManager.h"

namespace Strype {

	PanelManager::PanelManager()
	{
		m_Inspector = AddPanel<InspectorPanel>();

		for (const Ref<EditorPanel>& panel : m_Panels)
			panel->SetInspector(m_Inspector);
	}

	PanelManager::~PanelManager()
	{
		m_Panels.clear();
	}

	void PanelManager::OnImGuiRender()
	{
		for (const Ref<EditorPanel>& panel : m_Panels)
			panel->OnImGuiRender();
	}

	void PanelManager::OnEvent(Event& e)
	{
		for (const Ref<EditorPanel>& panel : m_Panels)
			panel->OnEvent(e);
	}

	void PanelManager::SetRoomContext(const Ref<Room>& context)
	{
		for (const Ref<EditorPanel>& panel : m_Panels)
			panel->SetRoomContext(context);

		m_ActiveRoom = context;
	}

	void PanelManager::OnProjectChanged()
	{
		for (const Ref<EditorPanel>& panel : m_Panels)
			panel->OnProjectChanged();
	}

}