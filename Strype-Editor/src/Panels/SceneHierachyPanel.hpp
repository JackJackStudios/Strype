#pragma once

#include "EditorPanel.hpp"
#include "InspectorPanel.hpp"

namespace Strype {

	class SceneHierachyPanel : public EditorPanel
	{
	public:
		SceneHierachyPanel();

		virtual void OnImGuiRender();
		virtual void SetInspector(Ref<InspectorPanel> panel) { m_Inspector = panel; }
		inline virtual void SetRoomContext(const Ref<Room>& room) { m_ActiveScene = room; m_Selection = {}; }

		Object GetSelected() const { return m_Selection; }
		void SetSelected(Object obj) { m_Selection = obj; m_Inspector->SetSelected(&m_Selection); }
		void RemoveSelected() { m_Selection = Object(); m_Inspector->SetSelected<void>(nullptr); }
	private:
		void OnInspectorRender(Object* select);
	private:
		Ref<InspectorPanel> m_Inspector;
		Ref<Room> m_ActiveScene;
		Object m_Selection;
	};

}