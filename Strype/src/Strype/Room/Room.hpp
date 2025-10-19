#pragma once

#include "Strype/Project/Asset.hpp"
#include "Strype/Core/UUID.hpp"
#include "Strype/Renderer/Renderer.hpp"

#include <box2d/box2d.h>
#include <Coral/ManagedObject.hpp>

namespace Strype {

	enum class RoomState
	{
		Editor,
		Runtime,
	};

	struct RoomInstance
	{
		AssetHandle ObjectHandle;
		float CurrentFrame = 0.0f;
		float AnimationSpeed = 1.0f;

		glm::vec2 Position = { 0.0f, 0.0f };
		glm::vec2 Scale = { 1.0f, 1.0f };
		float Rotation = 0.0f;

		glm::vec4 Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
		std::vector<Coral::ManagedObject*> CSharpObjects;
		b2BodyId RuntimeBody;
	};

	using InstanceID = UUID32;

	class Room : public Asset
	{
	public:
		void OnUpdate(float ts);
		void OnRender(Renderer* renderer);

		void OnResize(const glm::vec2& size) { m_Camera.SetProjection(size); }
		void TogglePause(bool toggle) { m_IsPaused = toggle; }

		void ToggleRuntime(bool toggle);

		InstanceID CreateInstance(const RoomInstance& instance)
		{
			uint32_t handle = (uint32_t)m_Indices.size();
			m_Indices.push_back((uint32_t)m_Objects.size());
			m_Reverse.push_back(handle);
			m_Objects.push_back(instance);
			return handle;
		}

		void DestroyInstance(InstanceID handle)
		{
			uint32_t index = m_Indices[handle];
			uint32_t last = (uint32_t)m_Objects.size() - 1;

			m_Objects[index] = m_Objects[last];
			uint32_t movedHandle = m_Reverse[last];
			m_Indices[movedHandle] = index;
			m_Reverse[index] = movedHandle;

			m_Reverse.pop_back();
			m_Reverse.pop_back();
		}

		RoomInstance& GetInstance(InstanceID handle)
		{
			return m_Objects[m_Indices[handle]];
		}

		bool InstanceExists(InstanceID handle)
		{
			return handle < m_Indices.size();
		}

		static AssetType GetStaticType() { return AssetType::Room; }
		virtual AssetType GetType() const override { return GetStaticType(); }

		bool IsActive() const { return m_RoomState == RoomState::Runtime && !m_IsPaused; }
		Camera& GetCamera() { return m_Camera; }

		std::vector<InstanceID>::iterator begin() { return m_Reverse.begin(); }
		std::vector<InstanceID>::iterator end() { return m_Reverse.end(); }
	private:
		RoomState   m_RoomState = RoomState::Editor;
		bool        m_IsPaused = false;

		glm::uvec2  m_Size;
		glm::vec3   m_BackgroundColour;
		float       m_Gravity = 10.0f;
		b2WorldId   m_PhysicsWorld = {};
		Camera      m_Camera;

		std::vector<RoomInstance> m_Objects;
		std::vector<uint32_t> m_Indices;
		std::vector<InstanceID> m_Reverse;

		friend Ref<Asset> load_room_asset(const std::filesystem::path&);
		friend void save_room_asset(Ref<Asset>, const std::filesystem::path&);
		friend class ContentBrowserPanel;
	};

}