#pragma once

#include "Strype/Events/Event.h"
#include "Strype/Core/Timestep.h"
#include "Strype/Renderer/Camera.h"

#include "Strype/Asset/Asset.h"

#include <entt/entt.hpp>

namespace Strype {

	class Object;

	class Room : public Asset
	{
	public:
		Room();
		~Room();

		Object CreateObject(std::string name = std::string());
		void DestroyObject(Object entity);

		void Clear();
		void OnUpdate(Timestep ts, Camera cam);

		const std::string& GetName() const { return m_Name; }

		virtual AssetType GetType() const override { return AssetType::Room; }
	private:
		entt::registry m_Registry;
		std::string m_Name = "Untitled";

		friend class Object;
		friend class SceneHierachyPanel;
		friend class RoomSerializer;
	};

}