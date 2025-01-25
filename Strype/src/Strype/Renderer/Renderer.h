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

		static void SetClearColor(const glm::vec4& color);
		static void Clear();

		static void BeginScene(Camera& camera);
		static void EndScene();


		static RenderAPI::API GetAPI() { return RenderAPI::GetAPI(); }
	private:
		static Scope<RenderAPI> s_RenderAPI;
	};

}