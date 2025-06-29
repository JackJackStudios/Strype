#pragma once

#include "Strype/Core/Event.hpp"
#include "Strype/Renderer/Camera.hpp"

#include "Strype/Asset/Asset.hpp"

#include "RoomInstance.hpp"

namespace Strype {

	enum class RoomState
	{
		Editor,
		Runtime,
		Paused
	};

	class Room : public Asset
	{
	public:
		Room();

		InstanceID InstantiatePrefab(AssetHandle prefab);

		bool InstanceExists(InstanceID obj) const { return obj < m_Objects.size(); }
		void DestroyInstance(InstanceID obj);
		void Clear() { m_Objects.clear(); }

		void OnUpdate(float ts);

		void StartRuntime();
		void StopRuntime();

		void CopyTo(Ref<Room>& room);
		void OnResize(const glm::vec2& dims);
		void OnEvent(Event& e);

		static AssetType GetStaticType() { return AssetType::Room; }
		virtual AssetType GetType() const override { return GetStaticType(); }

		RoomState GetState() const { return m_RoomState; }
		Camera& GetCamera() { return m_Camera; }
		RoomInstance& GetObject(InstanceID id) { return m_Objects[id]; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		void InitObject(RoomInstance& object);
	private:
		std::vector<RoomInstance> m_Objects;

		uint64_t m_Width = 720, m_Height = 360;
		glm::vec3 m_BackgroundColour;

		RoomState m_RoomState = RoomState::Editor;

		Camera m_Camera;
		float m_CameraSpeed = 5.0f;
		float m_ZoomLevel = 1.0f;

		friend class Prefab;
		friend class RoomInstance;
		friend class SceneHierachyPanel;
		friend class RoomSerializer;
		friend class ContentBrowserPanel;
	};

	static Ref<Room> s_PrefabRoom = CreateRef<Room>();

}