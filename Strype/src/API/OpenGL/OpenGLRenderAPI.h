#pragma once

#include "Strype/Renderer/RenderAPI.h"

namespace Strype {

	class OpenGLRenderAPI : public RenderAPI
	{
	public:
		virtual void Init(void* window) override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
	};


}