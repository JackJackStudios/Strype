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
		inline virtual void SetRoomContext(const Ref<Room>& room) { m_ActiveScene = room; m_Selection = 0; }

		InstanceID GetSelected() const { return m_Selection; }
		void SetSelected(InstanceID obj) { m_Selection = obj; m_Inspector->SetSelected(&m_Selection); }
		void RemoveSelected() { m_Selection = 0; m_Inspector->RemoveSelected(); }
	private:
		void OnInspectorRender(InstanceID* select);
	private:
		Ref<InspectorPanel> m_Inspector;
		Ref<Room> m_ActiveScene;
		InstanceID m_Selection;
	};

}