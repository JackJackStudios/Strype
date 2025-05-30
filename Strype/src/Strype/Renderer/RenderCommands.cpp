#include "stypch.h"
#include "Strype/Renderer/Renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
	void Renderer::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colour, const Ref<Sprite>& sprite)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		if (rotation != 0) transform = transform * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f });
		transform = transform * glm::scale(glm::mat4(1.0f), glm::make_vec3(size));

		s_RenderPipelines.Get<QuadPipeline>()->DrawPrimitive(transform, colour, RenderCaps::TextureCoords, sprite ? GetTextureSlot(sprite->GetTexture()) : 0.0f);
	}

	void Renderer::DrawString(const glm::vec3& position, const std::string& string, const glm::vec4& colour, const Ref<Font>& font)
	{
		Character character = font->GetCharacter('!'); 
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);

		glm::vec2 uvBL = character.UVOffset;
		glm::vec2 uvBR = character.UVOffset + glm::vec2(character.UVSize.x, 0.0f);
		glm::vec2 uvTR = character.UVOffset + character.UVSize;
		glm::vec2 uvTL = character.UVOffset + glm::vec2(0.0f, character.UVSize.y);

		s_RenderPipelines.Get<TextPipeline>()->DrawPrimitive(transform, colour, { uvBL, uvBR, uvTR, uvTL }, GetTextureSlot(font->GetAtlas()));
	}

}