#pragma once

#include "Strype/Renderer/RenderAPI.h"
#include "Strype/Renderer/Shader.h"
#include "Strype/Renderer/Camera.h"
#include "Strype/Renderer/Texture.h"

namespace Strype {

	class Renderer
	{
	public:
		static void Init(void* window);
		static void Shutdown();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void SetClearColour(const glm::vec4& colour);
		static void Clear();

		static void BeginScene(Camera& camera);
		static void EndScene();
		static void Flush();

		// Primitives
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture>& texture, const glm::vec4& tintColour = glm::vec4(1.0f));

		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colour);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture>& texture, const glm::vec4& tintColour = glm::vec4(1.0f));

		static RenderAPI::API GetAPI() { return RenderAPI::GetAPI(); }
	private:
		static Scope<RenderAPI> s_RenderAPI;
	private:
		static void FlushAndReset();
	};

}