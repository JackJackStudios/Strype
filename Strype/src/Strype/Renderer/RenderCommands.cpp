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
	
	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", s_SceneData->Matrix);
		shader->SetMat4("u_Transform", transform);

		vertexArray->Bind();
		s_RenderAPI->DrawIndexed(vertexArray, 0);
	}

}