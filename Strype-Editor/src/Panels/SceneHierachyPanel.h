#pragma once

#include "EditorPanel.h"

namespace Strype {

	class SceneHierachyPanel : public EditorPanel
	{
	public:
		SceneHierachyPanel();

		virtual void OnImGuiRender();
		virtual void OnEvent(Event& event) {};
		inline virtual void SetRoomContext(const Ref<Room>& room) { m_ActiveScene = room; m_Selection = {}; }
	
	private:
		Ref<Room> m_ActiveScene;
		Object m_Selection;
	};

}