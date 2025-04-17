#pragma once

#include "EditorPanel.h"
#include "EditorCamera.h"

namespace Strype {

	class RuntimePanel : public EditorPanel
	{
	public:
		RuntimePanel();

		void StartRuntime();
		void StopRuntime();

		virtual void OnUpdate(Timestep ts);
		virtual void OnImGuiRender();
		inline virtual void SetRoomContext(const Ref<Room>& room) { m_EditorRoom = room; }
	private:
		Ref<AGI::Framebuffer> m_Framebuffer;
		Ref<Room> m_RuntimeRoom;

		Ref<Room> m_EditorRoom;
		Ref<EditorCamera> m_Camera; //TODO: Change this to user camera

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	};

}