#pragma once

#include <Strype.hpp>

namespace Strype {

	class EditorPanel
	{
	public:
		virtual ~EditorPanel() = default;

		virtual void OnUpdate(float ts) {}
		virtual void OnImGuiRender() {}

		virtual void OnEvent(Event& event) {}
	public:
		std::string Title;
		bool Closing = false;
		bool ManualControl = false;
		ImGuiWindowFlags Flags = 0;
	protected:
		// Pointer to var in EditorSession
		Ref<Room> m_CurrentRoom;
		bool m_IsOpen = true;

		friend class EditorSession;
	};

};