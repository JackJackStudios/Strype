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
		Room() = default;

		Object CreateObject(glm::vec3 position = glm::vec3());
		void DestroyObject(Object entity);
		void Clear() { m_Registry.clear(); }

		void OnUpdateEditor(Timestep ts, Camera& cam);
		void OnUpdateRuntime(Timestep ts, Camera& cam);

		void OnRuntimeStart();
		void OnRuntimeStop();

		const std::string& GetName() const { return m_Name; }

		static AssetType GetStaticType() { return AssetType::Room; }
		virtual AssetType GetType() const override { return GetStaticType(); }

		template<typename TComponent>
		void CopyComponentIfExists(entt::entity dst, entt::registry& dstRegistry, entt::entity src)
		{
			if (m_Registry.all_of<TComponent>(src))
			{
				auto& srcComponent = m_Registry.get<TComponent>(src);
				dstRegistry.emplace_or_replace<TComponent>(dst, srcComponent);
			}
		}
	private:
		entt::registry m_Registry;
		std::string m_Name = "Untitled";

		friend class Object;
		friend class Prefab;
		friend class SceneHierachyPanel;
		friend class RoomSerializer;
	};

	static Ref<Room> s_PrefabRoom = CreateRef<Room>();

}