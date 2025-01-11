#pragma once

#include "Strype/Renderer/RenderAPI.h"

namespace Strype {

	class OpenGLRenderAPI : public RenderAPI
	{
	public:
		virtual void Init(void* window) override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
	};


}