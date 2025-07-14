#pragma once

#include "Camera.hpp"
#include "Sprite.hpp"

#include "RenderPipeline.hpp"
#include "Strype/Utils/PlatformUtils.hpp"

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
		static void Init();
		static void Shutdown();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void SetClearColour(const glm::vec4& colour);
		static void Clear();

		static void BeginRoom(Camera& camera);
		static void EndRoom();

		// Primitives
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colour, const Ref<Sprite>& sprite = nullptr, const Buffer& buf = Buffer());

		static std::unique_ptr<AGI::RenderContext>& GetContext() { return s_RenderContext; }
	private:
		static float GetTextureSlot(const AGI::Texture& texture);
		static void Flush();
		static void FlushAndReset();
		static void InitPipeline(RenderPipeline& pipeline);
	private:
		inline static std::unique_ptr<AGI::RenderContext> s_RenderContext;

		inline static std::array<AGI::Texture, RenderCaps::MaxTextureSlots> s_TextureSlots;
		inline static AGI::Texture s_WhiteTexture;
		inline static uint32_t s_TextureSlotIndex = 1; // 0 = white texture

		inline static RenderPipeline s_QuadPipeline;

		friend class Font;
	};

}