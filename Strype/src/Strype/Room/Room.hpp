#pragma once

#include "Strype/Events/Event.hpp"
#include "Strype/Core/Timestep.hpp"
#include "Strype/Events/ApplicationEvent.hpp"
#include "Strype/Renderer/Camera.hpp"

#include "Strype/Asset/Asset.hpp"
#include "Strype/Events/MouseEvent.hpp"

#include <entt/entt.hpp>
#include <box2d/box2d.h>

namespace Strype {

	class Object;
	struct ScriptComponent;

	enum class RoomState
	{
		Editor,
		Runtime,
		Paused
	};

	using ScriptContainer = std::vector<ScriptComponent>;

	class Room : public Asset
	{
	public:
		Room();

		Object CreateObject();
		void DestroyObject(Object entity);
		void Clear() { m_Registry.clear(); }

		bool ObjectExists(entt::entity obj) { return m_Registry.valid(obj); }

		void OnUpdate(Timestep ts);
		void OnResize(const glm::vec2& dims);
		void OnEvent(Event& e);

		void StartRuntime();
		void StopRuntime();

		void CopyTo(Ref<Room>& room);
		RoomState GetState() const { return m_RoomState; }
		Camera& GetCamera() { return m_Camera; }

		static AssetType GetStaticType() { return AssetType::Room; }
		virtual AssetType GetType() const override { return GetStaticType(); }

		template<typename TComponent>
		static void CopyComponent(entt::entity src, entt::registry& srcreg, entt::entity dest, entt::registry& destreg)
		{
			if (srcreg.all_of<TComponent>(src))
			{
				auto& srcComponent = srcreg.get<TComponent>(src);
				destreg.emplace_or_replace<TComponent>(dest, srcComponent);
			}
		}
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
	private:
		uint64_t m_Width = 720, m_Height = 360;
		glm::vec3 m_BackgroundColour;
		entt::registry m_Registry;
		b2WorldId m_PhysicsWorld = {0};
		RoomState m_RoomState = RoomState::Editor;

		Camera m_Camera;
		float m_CameraSpeed = 5.0f;
		float m_ZoomLevel = 1.0f;

		float m_Gravity = 10.0f;
		uint16_t m_PhysicsSubsteps = 4;

		friend class Object;
		friend class Prefab;
		friend class SceneHierachyPanel;
		friend class RoomSerializer;
		friend class ContentBrowserPanel;
	};

	static Ref<Room> s_PrefabRoom = CreateRef<Room>();

}