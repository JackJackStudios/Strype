#pragma once

#include <Strype.h>

#include "EditorPanel.h"
#include "InspectorPanel.h"

namespace Strype {

	class PanelManager
	{
	public:
		PanelManager();
		~PanelManager();

		void OnImGuiRender();
		void OnEvent(Event& e);

		void SetRoomContext(const Ref<Room>& context);
		void OnProjectChanged();

		Ref<InspectorPanel> GetInspector() { return m_Inspector; }

		template<typename TPanel, typename... TArgs>
		Ref<TPanel> AddPanel(TArgs&&... args)
		{
			Ref<TPanel> temp = CreateRef<TPanel>(std::forward<TArgs>(args)...);
			temp->SetRoomContext(m_ActiveRoom);
			temp->SetInspector(m_Inspector);
			m_Panels.emplace_back(temp);
			return temp;
		}

	private:
		std::vector<Ref<EditorPanel>> m_Panels;
		Ref<InspectorPanel> m_Inspector;
		Ref<Room> m_ActiveRoom;
	};

}