#pragma once

#include <Strype.h>

namespace Strype {

	class Panel
	{
	public:
		virtual ~Panel() = default;

		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		virtual const char* GetName() = 0;
	};

}