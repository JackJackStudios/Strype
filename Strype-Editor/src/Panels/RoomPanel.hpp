#pragma once

#include "Strype/Utils/ImguiUtils.hpp"
#include "Panels/EditorPanel.hpp"

#include <Strype.hpp>
#include <ImGuizmo.h>

namespace Strype {

	// rect = x, y, width, height
	static bool PointInRectangle(const glm::vec2& position, const glm::vec4& rect)
	{
		return rect.x < position.x && rect.x + rect.z > position.x &&
			rect.y < position.y && rect.y + rect.w > position.y;
	}

	class RoomPanel : public EditorPanel
	{
	public:
		RoomPanel();

		virtual void OnImGuiRender();
		virtual void OnUpdate(float ts);
		virtual void OnEvent(Event& e);
	private:
		void OnMouseScrolled(MouseScrolledEvent& e);
	private:
		AGI::Framebuffer m_Framebuffer;
		Room::InstanceID m_Selected;

		float m_CameraSpeed = 100.0f;

		glm::vec2 m_ViewportBounds[2];
		bool m_ViewportFocused = false;
		ImGuizmo::OPERATION m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
	};

}