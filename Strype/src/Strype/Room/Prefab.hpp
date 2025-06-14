#pragma once

#include "Object.hpp"

namespace Strype {

	class Prefab : public Asset
	{
	public:
		Prefab()
			: m_Object((entt::entity)0, s_PrefabRoom.get())
		{
		}

		Prefab(entt::entity handle)
			: m_Object(handle, s_PrefabRoom.get())
		{
		}

		Prefab(const Prefab& other) = default;

		void ConnectObject(Object obj) { m_ConnectedObjects.emplace_back(obj); }
		std::vector<Object>& GetConnectedObjects() { return m_ConnectedObjects; }

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) { return m_Object.AddComponent<T>(std::forward<Args>(args)...); }

		template<typename T>
		T& GetComponent() { return m_Object.GetComponent<T>(); }

		template<typename T>
		T& EnsureCurrent() { return m_Object.EnsureCurrent<T>(); }

		template<typename T>
		T* TryGetComponent() { return m_Object.TryGetComponent<T>(); }

		template<typename T>
		bool HasComponent() { return m_Object.HasComponent<T>(); }

		template<typename T>
		void RemoveComponent() { m_Object.RemoveComponent<T>(); }
		
		Object GetObject() { return m_Object; }
		void SetObject(Object obj) { m_Object = obj; }

		static AssetType GetStaticType() { return AssetType::Prefab; }
		virtual AssetType GetType() const override { return GetStaticType(); }
	private:
		Object m_Object;

		std::vector<Object> m_ConnectedObjects;
	};

}