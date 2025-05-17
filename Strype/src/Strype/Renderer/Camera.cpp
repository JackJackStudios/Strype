#include "stypch.h"
#include "Strype/Renderer/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Strype {

	Camera::Camera(const glm::vec2& size)
		: m_ViewMatrix(1.0f)
	{
		SetProjection(size);
	}

	void Camera::SetZoomLevel(float zoom)
	{
		m_ProjectionMatrix = glm::ortho(-m_AspectRatio * zoom, m_AspectRatio * zoom, -zoom, zoom);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera::SetProjection(const glm::vec2& size)
	{
		m_AspectRatio = size.x / size.y;
		m_ProjectionMatrix = glm::ortho(-m_AspectRatio, m_AspectRatio, -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera::UpdateMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), Position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(Rotation), glm::vec3(0, 0, 1));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

}