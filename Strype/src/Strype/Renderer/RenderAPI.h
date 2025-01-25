#pragma once

#include <glm/glm.hpp>

#include "Strype/Renderer/VertexArray.h"

namespace Strype {

	class RenderAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};
	public:
		virtual ~RenderAPI() = default;

		virtual void Init(void* window) = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) = 0;

		static API GetAPI() { return s_API; }
		static Scope<RenderAPI> Create();
	private:
		static API s_API;
	};

}