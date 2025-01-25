#pragma once

#include "Strype/Core/Base.h"
#include "Strype/Events/Event.h"
#include "Strype/Core/Timestep.h"

namespace Strype {

	class Layer
	{
	public:
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}
	};

}