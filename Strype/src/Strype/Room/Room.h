#pragma once

#include "Strype/Events/Event.h"
#include "Strype/Core/Timestep.h"
#include "Strype/Events/ApplicationEvent.h"
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

		bool ObjectExists(entt::entity obj) { return m_Registry.valid(obj); }

		void OnUpdate(Timestep ts, Camera& cam, bool isRuntime);

		void OnRuntimeStart();
		void OnRuntimeStop();

		static AssetType GetStaticType() { return AssetType::Room; }
		virtual AssetType GetType() const override { return GetStaticType(); }

		template<typename T, typename... Args>
		T& AddComponent(entt::entity handle, Args&&... args)
		{
			STY_CORE_ASSERT(!HasComponent<T>(), "Object already has component!");
			return m_Registry.emplace<T>(handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent(entt::entity handle)
		{
			STY_CORE_ASSERT(HasComponent<T>(), "Object does not have component!");
			return m_Registry.get<T>(handle);
		}

		template<typename T>
		bool HasComponent(entt::entity handle)
		{
			return m_Registry.all_of<T>(handle);
		}

		template<typename T>
		void RemoveComponent(entt::entity handle)
		{
			STY_CORE_ASSERT(HasComponent<T>(), "Object does not have component!");
			m_Registry.remove<T>(handle);
		}

		template<typename TComponent>
		static void CopyComponentIfExists(entt::entity src, entt::registry& srcreg, entt::entity dest, entt::registry& destreg)
		{
			if (srcreg.all_of<TComponent>(src))
			{
				auto& srcComponent = srcreg.get<TComponent>(src);
				destreg.emplace_or_replace<TComponent>(dest, srcComponent);
			}
		}
	private:
		entt::registry m_Registry;

		friend class Object;
		friend class Prefab;
		friend class SceneHierachyPanel;
		friend class RoomSerializer;
	};

	static Ref<Room> s_PrefabRoom = CreateRef<Room>();

}