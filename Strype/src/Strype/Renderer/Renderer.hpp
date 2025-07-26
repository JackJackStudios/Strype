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
		Renderer(AGI::Window* window);

		void Init();
		void Shutdown();
		void OnWindowResize(const glm::vec2& size);

		void SetClearColour(const glm::vec4& colour);
		void Clear();

		void BeginRoom(Camera& camera);
		void EndRoom();

		// Primitives
		void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colour, const Ref<Sprite>& sprite = nullptr, const Buffer& buf = Buffer(0));

		AGI::RenderContext* GetContext() { return m_RenderContext; }
		static Renderer* GetCurrent() { return m_CurrentContext; }
	private:
		float GetTextureSlot(const AGI::Texture& texture);
		void Flush();
		void FlushAndReset();
		void InitPipeline(RenderPipeline& pipeline);
	private:
		AGI::RenderContext* m_RenderContext;
		RenderPipeline m_QuadPipeline;

		std::array<AGI::Texture, RenderCaps::MaxTextureSlots> m_TextureSlots;
		AGI::Texture m_WhiteTexture;
		uint32_t m_TextureSlotIndex = 1; // 0 = white texture

		inline static thread_local Renderer* m_CurrentContext;
	};

}