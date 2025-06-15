#pragma once

#include "Strype/Core/Base.hpp"
#include "Strype/Core/Event.hpp"

namespace Strype {

	class Layer
	{
	public:
		virtual ~Layer() {}

		virtual void OnUpdate(float ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}
	};

}