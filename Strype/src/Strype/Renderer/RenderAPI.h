#pragma once

#include <glm/glm.hpp>

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
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		static API GetAPI() { return s_API; }
		static Scope<RenderAPI> Create();
	private:
		static API s_API;
	};

}