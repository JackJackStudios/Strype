#pragma once

#include "Strype/Events/Event.h"
#include "Strype/Core/Timestep.h"
#include "Strype/Renderer/CameraController.h"

#include <entt/entt.hpp>

namespace Strype {

	class Object;

	class Room
	{
	public:
		Room();
		~Room();

		Object CreateObject();

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);
	private:
		entt::registry m_Registry;
		CameraController m_CameraController;

		friend class Object;
	};

}