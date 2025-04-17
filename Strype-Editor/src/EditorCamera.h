#pragma once

#include "Strype/Renderer/Camera.h"
#include "Strype/Core/Timestep.h"

#include "Strype/Events/ApplicationEvent.h"
#include "Strype/Events/MouseEvent.h"

namespace Strype {

	class EditorCamera
	{
	public:
		EditorCamera(float width, float height);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);
		void OnResize(float width, float height);

		Camera& GetCamera() { return m_Camera; }
		const Camera& GetCamera() const { return m_Camera; }
		glm::vec2 GetPosition() const { return m_CameraPosition; }

		float GetWidth() const { return m_Width; }
		float GetHeight() const { return m_Height; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
	private:
		float m_Width;
		float m_Height;

		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		Camera m_Camera;

		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraSpeed = 5.0f;
	};

}