#pragma once

#include <Strype.h>

#include "EditorPanel.h"

namespace Strype {

	class PanelManager
	{
	public:
		PanelManager() = default;
		~PanelManager();

		void OnImGuiRender();
		void OnEvent(Event& e);

		void SetRoomContext(const Ref<Room>& context);

		template<typename TPanel, typename... TArgs>
		Ref<TPanel> AddPanel(TArgs&&... args)
		{
			Ref<TPanel> temp = CreateRef<TPanel>(std::forward<TArgs>(args)...);
			m_Panels.emplace_back(temp);
			return temp;
		}

	private:
		std::vector<Ref<EditorPanel>> m_Panels;
		Ref<Room> m_ActiveRoom;
	};

}