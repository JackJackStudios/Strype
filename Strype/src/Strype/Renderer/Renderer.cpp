#include "stypch.h"
#include "Strype/Renderer/Renderer.h"

namespace Strype {

	Scope<RenderAPI> Renderer::s_RenderAPI = RenderAPI::Create();
	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();

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

	void Renderer::BeginScene(Camera& camera)
	{
		s_SceneData->Matrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", s_SceneData->Matrix);
		shader->SetMat4("u_Transform", transform);

		vertexArray->Bind();
		s_RenderAPI->DrawIndexed(vertexArray);
	}

}