#include "stypch.hpp"
#include "Strype/Renderer/Renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Strype {

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		s_RenderContext->SetViewport(0, 0, width, height);
	}

	void Renderer::Clear()
	{
		s_RenderContext->Clear();
	}

	void Renderer::SetClearColour(const glm::vec4& colour)
	{
		s_RenderContext->SetClearColour(colour);
	}

	// Simple functions
	void Renderer::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colour, const Ref<Sprite>& sprite)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		if (rotation != 0) transform = transform * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f });
		transform = transform * glm::scale(glm::mat4(1.0f), glm::make_vec3(size));

		for (size_t i = 0; i < 4; i++)
		{
			s_QuadPipeline.SubmitAttribute("a_Position", transform * RenderCaps::VertexPositions[i]);
			s_QuadPipeline.SubmitAttribute("a_Colour", colour);
			s_QuadPipeline.SubmitAttribute("a_TexCoord", RenderCaps::TextureCoords[i]);
			s_QuadPipeline.SubmitAttribute("a_TexIndex", sprite ? GetTextureSlot(sprite->GetTexture()) : 0.0f);

			s_QuadPipeline.NextPoint();
		}

		s_QuadPipeline.IndexCount += 6;
	}

}