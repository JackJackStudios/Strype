#pragma once

#include "EditorPanel.h"
#include "InspectorPanel.h"

namespace Strype {

	class SceneHierachyPanel : public EditorPanel
	{
	public:
		SceneHierachyPanel();

		virtual void OnImGuiRender();
		virtual void SetInspector(Ref<InspectorPanel> panel) { m_Inspector = panel; }
		inline virtual void SetRoomContext(const Ref<Room>& room) { m_ActiveScene = room; m_Selection = {}; }
	private:
		void OnInspectorRender(Object* select);
	private:
		Ref<InspectorPanel> m_Inspector;
		Ref<Room> m_ActiveScene;
		Object m_Selection;
	};

}