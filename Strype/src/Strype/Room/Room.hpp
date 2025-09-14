#pragma once

#include "Strype/Core/Event.hpp"
#include "Strype/Renderer/Camera.hpp"
#include "Strype/Project/Asset.hpp"

#include "RoomInstance.hpp"

#include <box2d/box2d.h>

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
		Room(const Room& other);

		InstanceID CreateInstance(AssetHandle object);

		bool InstanceExists(InstanceID obj) const { return obj < m_Objects.size(); }
		void DestroyInstance(InstanceID obj);
		void Clear() { m_Objects.clear(); }

		void OnUpdate(float ts);

		void StartRuntime();
		void StopRuntime();

		void OnResize(const glm::vec2& dims);
		void OnEvent(Event& e);

		static AssetType GetStaticType() { return AssetType::Room; }
		virtual AssetType GetType() const override { return GetStaticType(); }

		RoomState GetState() const { return m_RoomState; }
		Camera& GetCamera() { return m_Camera; }
		RoomInstance& GetObject(InstanceID id) { return m_Objects[id]; }

		std::vector<RoomInstance>::iterator begin() { return m_Objects.begin(); }
		std::vector<RoomInstance>::iterator end() { return m_Objects.end(); }
	private:
		void OnMouseScrolled(MouseScrolledEvent& e);
	private:
		std::vector<RoomInstance> m_Objects;

		b2WorldId m_Physics;
		float m_Gravity = 10.0f;

		uint64_t m_Width = 720, m_Height = 360;
		glm::vec3 m_BackgroundColour;

		RoomState m_RoomState = RoomState::Editor;

		Camera m_Camera;
		float m_CameraSpeed = 5.0f;
		float m_ZoomLevel = 1.0f;

		friend class RoomSerializer;
		friend class ContentBrowserPanel;
	};

}