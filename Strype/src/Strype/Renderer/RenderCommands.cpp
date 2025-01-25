#include "stypch.h"
#include "Strype/Renderer/Renderer.h"

namespace Strype {

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		s_RenderAPI->SetViewport(0, 0, width, height);
	}

	void Renderer::Clear()
	{
		s_RenderAPI->Clear();
	}

	void Renderer::SetClearColor(const glm::vec4& color)
	{
		s_RenderAPI->SetClearColor(color);
	}

}