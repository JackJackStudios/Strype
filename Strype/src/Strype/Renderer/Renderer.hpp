#pragma once

#include "Strype/Renderer/Camera.hpp"
#include "Strype/Renderer/RenderPipeline.hpp"
#include "Strype/Renderer/Sprite.hpp"
#include "Strype/Renderer/Font.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/gtc/matrix_transform.hpp>

namespace Strype {

	struct RenderCaps
	{
		static constexpr uint32_t MaxPrimitives = 20000;
		static constexpr uint32_t MaxVertices = MaxPrimitives * 4;
		static constexpr uint32_t MaxIndices = MaxPrimitives * 6;
		
		static constexpr uint32_t MaxTextureSlots = 32;
		static constexpr std::array<std::string_view, 4> RequiredAttrs = { "a_Position", "a_Colour", "a_TexCoord", "a_TexIndex" };
		static constexpr TexCoords TextureCoords = { glm::vec2(0.0f, 0.0f), glm::vec2(1.0f,  0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f) };
		static constexpr glm::vec4 VertexPositions[] = {
			{ -0.5f, -0.5f, 0.0f, 1.0f }, // Bottom-left
			{  0.5f, -0.5f, 0.0f, 1.0f }, // Bottom-right
			{  0.5f,  0.5f, 0.0f, 1.0f }, // Top-right
			{ -0.5f,  0.5f, 0.0f, 1.0f }  // Top-left
		};
	};

	class Renderer
	{
	public:
		Renderer(AGI::Window* window);
		~Renderer();

		void Init();
		void OnWindowResize(const glm::vec2& size);
		void SetClearColour(const glm::vec3& colour);

		void BeginFrame();
		void EndFrame();

		void BeginRoom(Camera& camera);
		void EndRoom();

		// Primitives
		void DrawQuad(const glm::mat4& transform, const glm::vec4& colour, float slotIndex, TexCoords texcoords = RenderCaps::TextureCoords);
		void DrawSprite(const glm::vec3& position, const glm::vec2& scale, float rotation, const glm::vec4& colour, Ref<Sprite> sprite = nullptr, float frame = 0);
		void DrawText(const glm::vec3& position, const std::string& text, Ref<Font> font);

		AGI::RenderContext* GetContext() const { return m_RenderContext; }
		AGI::Window* GetWindow() const { return m_RenderContext->GetBoundWindow(); }
		FT_Library GetFreetypeLib() const { return m_FreetypeLib; }

		static constexpr glm::mat4 GetTransform(const glm::vec3& position, const glm::vec2& scale, float rotation)
		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
			if (rotation != 0) transform = transform * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f });
			transform *= glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 0.0f));

			return transform;
		}

		static Renderer* GetCurrent() { return m_CurrentContext; }
	private:
		float GetTextureSlot(AGI::Texture texture);

		void Flush();
		void FlushAndReset();
		void InitPipeline(RenderPipeline& pipeline, const std::filesystem::path& filepath);
	private:
		AGI::RenderContext* m_RenderContext;
		FT_Library m_FreetypeLib = nullptr;

		std::array<AGI::Texture, RenderCaps::MaxTextureSlots> m_TextureSlots;
		AGI::Texture m_WhiteTexture;
		uint32_t m_TextureSlotIndex = 1; // 0 = white texture

		// Enter other pipelines here...
		RenderPipeline m_QuadPipeline;
		RenderPipeline m_TextPipeline;

		inline static thread_local Renderer* m_CurrentContext;
	};

}