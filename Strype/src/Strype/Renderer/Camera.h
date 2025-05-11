#pragma once

#include <glm/glm.hpp>

namespace Strype {

	class Camera
	{
	public:
		Camera(const glm::vec2& size);

		void SetProjection(const glm::vec2& size);

		void UpdateMatrix();

		const glm::mat4 GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4 GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }
	public:
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		float Rotation = 0.0f;
	private:
		float m_AspectRatio;

		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;

	};

}