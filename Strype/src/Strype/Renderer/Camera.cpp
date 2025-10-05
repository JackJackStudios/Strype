#include "stypch.hpp"
#include "Strype/Renderer/Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Strype {

	void Camera::SetZoomLevel(float zoom)
	{
		m_ZoomLevel = zoom;
		SetProjection(m_Size);
	}

	void Camera::SetProjection(const glm::vec2& size)
	{
		m_Size = size;
		float halfWidth = (size.x * 0.5f) / m_ZoomLevel;
		float halfHeight = (size.y * 0.5f) / m_ZoomLevel;

		m_AspectRatio = size.x / size.y;
		m_ProjectionMatrix = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight);
	}

	void Camera::UpdateMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { Position.x, Position.y, 0.0f }) *
			glm::rotate(glm::mat4(1.0f), glm::radians(Rotation), glm::vec3(0, 0, 1));

		m_ViewMatrix = glm::inverse(transform);
	}

}