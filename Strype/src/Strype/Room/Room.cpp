#include "stypch.hpp"
#include "Room.hpp"

#include "Strype/Renderer/Renderer.hpp"
#include "Strype/Renderer/Sprite.hpp"

#include "Strype/Room/RoomInstance.hpp"
#include "Strype/Room/Object.hpp"
#include "Strype/Script/ScriptEngine.hpp"
#include "Strype/Project/Project.hpp"
#include "Strype/Core/Input.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Strype {

	Room::Room()
		: m_Camera({ 1280.0f, 720.0f })
	{
	}

	void Room::OnUpdate(float ts)
	{
		Renderer::BeginRoom(m_Camera);

		Renderer::DrawQuad({ 0.0f, 0.0f, 0.0f }, { m_Width, m_Height }, 0.0f, glm::make_vec4(m_BackgroundColour), nullptr, Buffer(0));

		if (m_RoomState == RoomState::Editor)
		{
			if (Input::IsKeyHeld(KeyCode::A))
			{
				m_Camera.Position.x -= m_CameraSpeed * ts;
			}
			else if (Input::IsKeyHeld(KeyCode::D))
			{
				m_Camera.Position.x += m_CameraSpeed * ts;
			}

			if (Input::IsKeyHeld(KeyCode::W))
			{
				m_Camera.Position.y += m_CameraSpeed * ts;
			}
			else if (Input::IsKeyHeld(KeyCode::S))
			{
				m_Camera.Position.y -= m_CameraSpeed * ts;
			}

			m_Camera.UpdateMatrix();
		}

		auto& scriptEngine = Project::GetScriptEngine();

		for (auto& object : m_Objects)
		{
			Renderer::DrawQuad(
				glm::make_vec3(object.Position),
				object.Scale,
				object.Rotation,
				object.Colour,
				Project::GetAsset<Sprite>(Project::GetAsset<Object>(object.PrefabHandle)->TextureHandle),
				Buffer((float)(object.m_Handle+1))
			);

			if (m_RoomState == RoomState::Runtime)
			{
				//object.Emitter->SetPos(object.Position);
				object.Instance.Invoke("OnUpdate", ts);
			}
		}

		Renderer::EndRoom();
	}

	void Room::StartRuntime()
	{
		if (m_RoomState == RoomState::Runtime)
		{
			STY_CORE_WARN("Cannot start room ({}) twice!", Project::GetFilePath(Handle).filename());
			return;
		}

		Project::SetActiveRoom(this);
		STY_CORE_INFO("Staring room \"{}\" ", Project::GetFilePath(Handle).stem());

		auto& scriptEngine = Project::GetScriptEngine();

		for (auto& object : m_Objects)
		{
			ScriptID scriptID = Project::GetAsset<Object>(object.PrefabHandle)->ClassID;

			if (scriptEngine->IsValidScript(scriptID))
			{
				object.Instance = scriptEngine->CreateInstance(scriptID, object.m_Handle, object.PrefabHandle);
				object.Instance.Invoke("OnCreate");
			}
			else
			{
				STY_CORE_WARN("Object '{}' has an invalid script id ({})", (uint32_t)object, (uint64_t)scriptID);
			}
		}

		m_RoomState = RoomState::Runtime;
	}

	void Room::StopRuntime()
	{
		if (m_RoomState == RoomState::Editor)
		{
			STY_CORE_WARN("Cannot stop room ({}) twice", Project::GetFilePath(Handle).filename().string());
			return;
		}

		auto& scriptEngine = Project::GetScriptEngine();
		Project::SetActiveRoom(nullptr);

		for (auto& object : m_Objects)
		{
			object.Instance.Invoke("OnDestroy");
			scriptEngine->DestroyInstance(object.Instance);
		}

		m_RoomState = RoomState::Editor;
	}

	void Room::CopyTo(Ref<Room>& room)
	{
		room->Handle = Handle;

		room->m_Width = m_Width;
		room->m_Height = m_Height;
		room->m_BackgroundColour = m_BackgroundColour;

		room->m_Objects = m_Objects;
	}

	InstanceID Room::InstantiatePrefab(AssetHandle prefab)
	{
		return m_Objects.emplace_back(UUID32(m_Objects.size()), prefab, this).m_Handle;
	}

	void Room::DestroyInstance(InstanceID obj)
	{
		STY_CORE_VERIFY(false, "Not implemented");
	}

	void Room::OnResize(const glm::vec2& size)
	{
		m_Camera.SetProjection(size);
	}

	void Room::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		if (m_RoomState == RoomState::Editor)
			dispatcher.Dispatch<MouseScrolledEvent>(STY_BIND_EVENT_FN(Room::OnMouseScrolled));
	}

	bool Room::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetZoomLevel(m_ZoomLevel);

		return true;
	}

}