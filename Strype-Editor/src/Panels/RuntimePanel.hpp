#pragma once

#include "EditorPanel.hpp"

namespace Strype {

	class RuntimePanel : public EditorPanel
	{
	public:
		RuntimePanel(Ref<Room>& room);
		~RuntimePanel();

		virtual void OnUpdate(Timestep ts);
		virtual void OnImGuiRender();
	private:
		Ref<AGI::Framebuffer> m_Framebuffer;
		Ref<Room> m_Room;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	};

}