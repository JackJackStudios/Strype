#include "stypch.h"
#include "Strype/Renderer/Renderer.h"

namespace Strype {

	Scope<RenderAPI> Renderer::s_RenderAPI = RenderAPI::Create();

	void Renderer::Init(void* window)
	{
		s_RenderAPI->Init(window);
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