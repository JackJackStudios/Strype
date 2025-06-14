#pragma once

#include "Strype/Core/Base.hpp"
#include "Strype/Events/Event.hpp"
#include "Strype/Core/Timestep.hpp"

namespace Strype {

	class Layer
	{
	public:
		virtual ~Layer() {}

		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}
	};

}