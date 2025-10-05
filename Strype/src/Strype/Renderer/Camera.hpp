#pragma once

#include <glm/glm.hpp>

namespace Strype {

	class Camera
	{
	public:
		void SetProjection(const glm::vec2& size);
		void SetZoomLevel(float zoom);

		float GetZoomLevel() const { return m_ZoomLevel; }
		glm::vec2 GetSize() const { return m_Size; }
		glm::vec2 GetHalfSize() const { return { m_Size.x / 2, m_Size.y / 2 }; }

		void UpdateMatrix();

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }
	public:
		glm::vec2 Position = { 0.0f, 0.0f };
		float Rotation = 0.0f;
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		glm::vec2 m_Size;

		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;

	};

}