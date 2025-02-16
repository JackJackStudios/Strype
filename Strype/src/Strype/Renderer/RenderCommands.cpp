#include "stypch.h"
#include "Strype/Renderer/Renderer.h"

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
	void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, colour);
	}

	void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture>& texture, const glm::vec4& tintColour)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tintColour);
	}

	void Renderer::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& colour)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, colour);
	}

	void Renderer::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture>& texture, const glm::vec4& tintColour)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tintColour);
	}

}