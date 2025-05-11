#pragma once

#include <AGI/agi.h>

#include "Camera.h"

namespace Strype {

	struct RendererData
	{
		static constexpr uint32_t MaxQuads = 20000;
		static constexpr uint32_t MaxVertices = MaxQuads * 4;
		static constexpr uint32_t MaxIndices = MaxQuads * 6;
		static constexpr uint32_t MaxTextureSlots = 32; // TODO: RenderCaps
		static constexpr size_t QuadVertexCount = 4;

		static constexpr glm::vec2 TextureCoords[] = { { 0.0f,  0.0f }, { 1.0f,  0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		
		void* QuadVertexBufferBase = nullptr; 
		void* QuadVertexBufferPtr = nullptr;

		uint32_t QuadIndexCount = 0;
		std::array<std::shared_ptr<AGI::Texture>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		std::shared_ptr<AGI::VertexArray> QuadVertexArray;
		std::shared_ptr<AGI::VertexBuffer> QuadVertexBuffer;
		std::shared_ptr<AGI::Shader> TextureShader;
		std::shared_ptr<AGI::Texture> WhiteTexture;

		glm::vec4 QuadVertexPositions[4];
		AGI::BufferLayout Layout;
		std::unordered_map<std::string, uint32_t> EngineAttributes;
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

		static void DrawBasicQuad(const glm::mat4& transform, const glm::vec4& colour, const glm::vec2 texcoords[], const Ref<AGI::Texture>& texture = nullptr);

		// Primitives
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour, const Ref<AGI::Texture>& texture = nullptr);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colour, const Ref<AGI::Texture>& texture = nullptr);

		// Shorthands for 2D coords
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour, const Ref<AGI::Texture>& texture = nullptr);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& colour, const Ref<AGI::Texture>& texture = nullptr);
	private:
		inline static std::unique_ptr<AGI::RenderAPI> s_RenderAPI;
	private:
		static void Flush();
		static void FlushAndReset();
	};

}