#include "stypch.hpp"
#include "Strype/Renderer/Renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Strype {

	void Renderer::OnWindowResize(const glm::vec2& size)
	{
		m_RenderContext->SetViewport(0, 0, size.x, size.y);
	}

	void Renderer::BeginFrame()
	{
		m_RenderContext->BeginFrame();
	}

	void Renderer::EndFrame()
	{
		m_RenderContext->EndFrame();
	}

	void Renderer::SetClearColour(const glm::vec3& colour)
	{
		m_RenderContext->SetClearColour({ colour.x, colour.y, colour.z, 1.0f });
	}

	constexpr glm::mat4 Renderer::GetTransform(const glm::vec3& position, const glm::vec2& scale, float rotation)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		if (rotation != 0) transform = transform * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f });
		transform *= glm::scale(glm::mat4(1.0f), glm::make_vec3(scale));

		return transform;
	}

	void Renderer::DrawQuad(const glm::mat4& transform, const glm::vec4& colour, float slotIndex, TexCoords texcoords)
	{
		for (size_t i = 0; i < 4; i++)
		{
			m_QuadPipeline.SubmitAttribute("a_Position", transform * RenderCaps::VertexPositions[i]);
			m_QuadPipeline.SubmitAttribute("a_Colour", colour);
			m_QuadPipeline.SubmitAttribute("a_TexCoord", texcoords[i]);
			m_QuadPipeline.SubmitAttribute("a_TexIndex", slotIndex);

			m_QuadPipeline.NextPoint();
		}

		m_QuadPipeline.IndexCount += 6;
	}

	// Simple functions
	void Renderer::DrawSprite(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colour, Ref<Sprite> sprite, float frame)
	{
		glm::mat4 transform = GetTransform(position, size, rotation);
		if (sprite) transform *= glm::scale(glm::mat4(1.0f), glm::make_vec3(sprite->GetFrameSize()));

		TexCoords texCoords = sprite ? sprite->GetTexCoords(frame) : RenderCaps::TextureCoords;
		float slotIndex = sprite ? GetTextureSlot(sprite) : 0.0f;

		DrawQuad(transform, colour, slotIndex, texCoords);
	}

}