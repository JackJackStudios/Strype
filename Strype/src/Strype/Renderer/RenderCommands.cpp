#include "stypch.h"
#include "Strype/Renderer/Renderer.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Strype {

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		s_RenderAPI->SetViewport(0, 0, width, height);
	}

	void Renderer::Clear()
	{
		s_RenderAPI->Clear();
	}

	void Renderer::SetClearColour(const glm::vec4& colour)
	{
		s_RenderAPI->SetClearColour(colour);
	}

	// Simple functions
	void Renderer::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour, const Ref<AGI::Texture>& texture)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawBasicQuad(transform, colour, RendererData::TextureCoords, texture);
	}

	void Renderer::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colour, const Ref<AGI::Texture>& texture)
	{
		if (rotation == 0)
			return DrawQuad(position, size, colour, texture);
		
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawBasicQuad(transform, colour, RendererData::TextureCoords, texture);
	}

	void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour, const Ref<AGI::Texture>& texture)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, colour, texture);
	}

	void Renderer::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& colour, const Ref<AGI::Texture>& texture)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, colour, texture);
	}

}