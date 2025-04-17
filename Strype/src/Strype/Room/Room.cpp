#include "stypch.h"
#include "Room.h"

#include "Strype/Renderer/Renderer.h"
#include "Strype/Renderer/Sprite.h"

#include "Strype/Room/Object.h"
#include "Strype/Room/Components.h"
#include "Strype/Project/Project.h"
#include "Strype/Core/Input.h"

#include <glm/glm.hpp>

namespace Strype {
	
	Room::Room()
		: m_Camera({ 1280.0f, 720.0f })
	{
	}

	void Room::OnUpdate(Timestep ts)
	{
		Renderer::BeginRoom(m_Camera);

		m_Registry.view<Transform, SpriteRenderer>().each([](auto entity, Transform& trans, SpriteRenderer& sprite) {
			if (Project::IsAssetLoaded(sprite.Texture))
				Renderer::DrawRotatedQuad(trans.Position, trans.Scale, trans.Rotation, Project::GetAsset<Sprite>(sprite.Texture)->Texture, sprite.Colour);
			else
				Renderer::DrawRotatedQuad(trans.Position, trans.Scale, trans.Rotation, sprite.Colour);
		});

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

		if (m_RoomState == RoomState::Runtime)
		{
			auto& scriptEngine = Project::GetScriptEngine();

			m_Registry.view<ScriptComponent>().each([&](auto entity, ScriptComponent& script) {
				if (scriptEngine->IsValidScript(script.ClassID) && script.Instance.IsValid())
				{
					script.Instance.Invoke<float>("OnUpdate", ts);
				}
				else if (script.ClassID != 0)
				{
					STY_CORE_ERROR("Object '{}' has invalid script!", (uint32_t)entity);
				}
			});
		}

		Renderer::EndRoom();
	}

	void Room::StartRuntime()
	{
		if (m_RoomState == RoomState::Runtime)
		{
			STY_CORE_WARN("Cannot start room ({}) twice!", Project::GetFilePath(Handle).filename().string());
			return;
		}

		Project::SetActiveRoom(Project::GetAsset<Room>(Handle));

		auto& scriptEngine = Project::GetScriptEngine();

		m_Registry.view<ScriptComponent>().each([&](auto entity, ScriptComponent& script) {
			if (scriptEngine->IsValidScript(script.ClassID))
			{
				script.Instance = scriptEngine->CreateInstance(script.ClassID);
				script.Instance.Invoke("OnCreate");
			}
			else if (script.ClassID != 0)
			{
				STY_CORE_WARN("Object '{}' has an invalid script id ({})", (uint32_t)entity, (uint64_t)script.ClassID);
			}
		});

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

		m_Registry.view<ScriptComponent>().each([&](auto entity, ScriptComponent& script) {
			script.Instance.Invoke("OnDestroy");
			scriptEngine->DestroyInstance(script.Instance);
		});

		m_RoomState = RoomState::Editor;
	}

	void Room::CopyTo(Ref<Room>& room)
	{
		room->Handle = Handle;

		for (auto entity : m_Registry.storage<entt::entity>())
		{
			Object temp{ entity, this };

			Object::CopyInto(temp, room->CreateObject());
		}
	}

	Object Room::CreateObject(glm::vec3 position)
	{
		Object object{ m_Registry.create(), this };
		object.AddComponent<Transform>(position);
		return object;
	}

	void Room::DestroyObject(Object entity)
	{
		m_Registry.destroy(entity);
	}

	void Room::OnResize(const glm::vec2& size)
	{
		m_Camera.SetProjection(size);
	}

}