#pragma once

#include "Strype/Events/Event.h"
#include "Strype/Core/Timestep.h"
#include "Strype/Renderer/Camera.h"

#include <entt/entt.hpp>

namespace Strype {

	class Object;

	class Room
	{
	public:
		Room();
		~Room();

		Object CreateObject();

		void OnUpdate(Timestep ts, Camera cam);
	private:
		entt::registry m_Registry;

		friend class Object;
		friend class SceneHierachyPanel;
	};

}