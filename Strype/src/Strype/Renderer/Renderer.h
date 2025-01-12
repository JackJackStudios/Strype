#pragma once

#include "Strype/Renderer/RenderAPI.h"
#include "Strype/Renderer/Shader.h"
#include "Strype/Renderer/Camera.h"

namespace Strype {

	class Renderer
	{
	public:
		static void Init(void* window);
		static void SetClearColor(const glm::vec4& color);
		static void Clear();

		static void BeginScene(Camera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		static RenderAPI::API GetAPI() { return RenderAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 Matrix;
		};

		static Scope<SceneData> s_SceneData;
		static Scope<RenderAPI> s_RenderAPI;
	};

}