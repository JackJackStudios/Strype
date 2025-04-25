#pragma once

#include "Strype/Room/Room.h"

#include <entt/entt.hpp>

namespace Strype {

	class Object
	{
	public:
		Object() = default;
		Object(const Object& other) = default;
		Object(entt::entity handle, Room* room)
			: m_Handle(handle), m_Room(room)
		{
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			STY_CORE_VERIFY(!HasComponent<T>(), "Object already has component!");
			return m_Room->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			STY_CORE_VERIFY(HasComponent<T>(), "Object does not have component!");
			return m_Room->m_Registry.get<T>(m_Handle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Room->m_Registry.all_of<T>(m_Handle);
		}

		template<typename T>
		T* TryGetComponent()
		{
			return HasComponent<T>() ? &GetComponent<T>() : nullptr;
		}

		template<typename T>
		T& EnsureCurrent()
		{
			return HasComponent<T>() ? GetComponent<T>() : AddComponent<T>();
		}

		template<typename T>
		void RemoveComponent()
		{
			STY_CORE_VERIFY(HasComponent<T>(), "Object does not have component!");
			m_Room->m_Registry.remove<T>(m_Handle);
		}

		static Object Copy(Object src, Ref<Room> targetroom);
		static void CopyInto(Object src, Object dest);

		operator uint32_t() const { return (uint32_t)m_Handle; }
		operator entt::entity() const { return m_Handle; }

		bool operator!=(const entt::entity& other) { return m_Handle != other; }
		bool operator==(const entt::entity& other) { return m_Handle == other; }
		operator bool() const { return m_Handle != entt::null; }
	private:
		entt::entity m_Handle{ entt::null };
		Room* m_Room = nullptr;

		friend class Room;
	};

}