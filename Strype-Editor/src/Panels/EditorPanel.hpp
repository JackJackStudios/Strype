#pragma once

#include <Strype.hpp>

namespace Strype {

	class InspectorPanel;

	class EditorPanel
	{
	public:
		std::string Title;
		bool Closing = false;
		ImGuiWindowFlags Flags = 0;

		virtual ~EditorPanel() = default;

		virtual void OnUpdate(float ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}
		virtual void OnProjectChanged() {};
	protected:
		Ref<Room> m_CurrentRoom;
		Ref<InspectorPanel> m_Inspector;
		bool m_IsOpen = true;

		friend class PanelManager;
	};

}