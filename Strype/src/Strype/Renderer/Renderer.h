#pragma once

#include <agi.h>

#include "Camera.h"

namespace Strype {

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
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<AGI::Texture>& texture, const glm::vec4& tintColour = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<AGI::Texture>& texture, const glm::vec2 texcoords[], const glm::vec4& tintColour = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colour);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<AGI::Texture>& texture, const glm::vec4& tintColour = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<AGI::Texture>& texture, const glm::vec2 texcoords[], const glm::vec4& tintColour = glm::vec4(1.0f));

		// Shorthands for 2D coords
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<AGI::Texture>& texture, const glm::vec4& tintColour = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& colour);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<AGI::Texture>& texture, const glm::vec4& tintColour = glm::vec4(1.0f));
	private:
		static Scope<AGI::RenderAPI> s_RenderAPI;
	private:
		static void Flush();
		static void FlushAndReset();
	};

}