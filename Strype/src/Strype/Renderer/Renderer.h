#pragma once

#include "Camera.h"
#include "Strype/Utils/TypeMap.h"
#include "RenderPipeline.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Strype {

	struct RenderCaps
	{
		static constexpr uint32_t MaxPrimitives = 20000;
		static constexpr uint32_t MaxVertices = MaxPrimitives * 4;
		static constexpr uint32_t MaxIndices = MaxPrimitives * 6;
		
		static constexpr uint32_t MaxTextureSlots = 32;
		static constexpr std::array<std::string_view, 4> RequiredAttrs = { "a_Position", "a_Colour", "a_TexCoord", "a_TexIndex" };
		static constexpr glm::vec2 TextureCoords[] = { { 0.0f,  0.0f }, { 1.0f,  0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
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
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colour, const Ref<AGI::Texture>& texture = nullptr);
		
		template<typename T>
		static void SubmitAttribute(const std::string& name, const std::any& value)
		{
			RenderPipelines.Get<QuadPipeline>()->SubmitAttribute(name, value);
		}
	private:
		static float GetTextureSlot(const std::shared_ptr<AGI::Texture>& texture);
		static void Flush();
		static void FlushAndReset();
	private:
		inline static std::unique_ptr<AGI::RenderAPI> s_RenderAPI;

		inline static std::array<std::shared_ptr<AGI::Texture>, RenderCaps::MaxTextureSlots> TextureSlots;
		inline static std::shared_ptr<AGI::Texture> WhiteTexture;
		inline static uint32_t TextureSlotIndex = 1; // 0 = white texture

		inline static TypeMap<Ref<RenderPipeline>> RenderPipelines;
	};

}