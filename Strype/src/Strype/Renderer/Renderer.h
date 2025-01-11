#pragma once

#include "Strype/Renderer/RenderAPI.h"

namespace Strype {

	class Renderer
	{
	public:
		static void Init(void* window);
		static void SetClearColor(const glm::vec4& color);
		static void Clear();

		static RenderAPI::API GetAPI() { return RenderAPI::GetAPI(); }
	private:
		static Scope<RenderAPI> s_RenderAPI;
	};

}