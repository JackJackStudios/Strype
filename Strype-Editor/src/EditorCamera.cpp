#include "stypch.h"
#include "EditorCamera.h"

#include "Strype/Core/Input.h"
#include "Strype/Core/KeyCodes.h"

namespace Strype {

	EditorCamera::EditorCamera(float width, float height)
		: m_AspectRatio(width / height), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel)
	{
	}

	void EditorCamera::OnUpdate(Timestep ts)
	{
		if (Input::IsKeyHeld(KeyCode::A))
		{
			m_CameraPosition.x -= m_CameraSpeed * ts;
		}
		else if (Input::IsKeyHeld(KeyCode::D))
		{
			m_CameraPosition.x += m_CameraSpeed * ts;
		}

		if (Input::IsKeyHeld(KeyCode::W))
		{
			m_CameraPosition.y += m_CameraSpeed * ts;
		}
		else if (Input::IsKeyHeld(KeyCode::S))
		{
			m_CameraPosition.y -= m_CameraSpeed * ts;
		}
		
		m_Camera.SetPosition(m_CameraPosition);

		m_CameraSpeed = m_ZoomLevel;
	}

	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(STY_BIND_EVENT_FN(EditorCamera::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(STY_BIND_EVENT_FN(EditorCamera::OnWindowResized));
	}

	void EditorCamera::OnResize(float width, float height)
	{
		m_AspectRatio = width / height;
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	bool EditorCamera::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}

	bool EditorCamera::OnWindowResized(WindowResizeEvent& e)
	{
		OnResize((float)e.GetWidth(), (float)e.GetHeight());
		return false;
	}

}