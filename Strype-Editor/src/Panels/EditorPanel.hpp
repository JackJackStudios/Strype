#pragma once

#include <Strype.hpp>

namespace Strype {

	class InspectorPanel;

	class EditorPanel
	{
	public:
		virtual ~EditorPanel() = default;

		virtual void OnUpdate(float ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}
		virtual void SetRoomContext(const Ref<Room>& room) {};
		virtual void OnProjectChanged() {};

		virtual void SetInspector(Ref<InspectorPanel> panel) {}
	};

}