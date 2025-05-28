#include "stypch.h"
#include "Room.h"

#include "Strype/Renderer/Renderer.h"
#include "Strype/Renderer/Sprite.h"

#include "Strype/Room/Object.h"
#include "Strype/Room/Components.h"
#include "Strype/Project/Project.h"
#include "Strype/Core/Input.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Strype {

	Room::Room()
		: m_Camera({ 1280.0f, 720.0f })
	{
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

	void Room::OnUpdate(Timestep ts)
	{
		Renderer::BeginRoom(m_Camera);

		Renderer::DrawQuad({ 0.0f, 0.0f, 0.0f }, { m_Width, m_Height }, 0.0f, glm::make_vec4(m_BackgroundColour));

		m_Registry.view<Transform, SpriteRenderer>().each([](auto entity, Transform& trans, SpriteRenderer& sprite) {
			Renderer::DrawQuad(
				glm::make_vec3(trans.Position), 
				trans.Scale, 
				trans.Rotation, 
				sprite.Colour, 
				Project::GetAsset<Sprite>(sprite.Texture)
			);
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

			m_Registry.view<ScriptContainer>().each([&](auto entity, ScriptContainer& container) {
				for (auto& script : container) 
				{
					if (scriptEngine->IsValidScript(script.ClassID))
						script.Instance.Invoke<float>("OnUpdate", ts);
				}
			});

			b2World_Step(m_PhysicsWorld, ts, m_PhysicsSubsteps);

			m_Registry.view<RigidBodyComponent, Transform>().each([&](auto entity, RigidBodyComponent& rigid, Transform& transform) {
				const auto& position = b2Body_GetPosition(rigid.RuntimeBody);

				transform.Position.x = position.x;
				transform.Position.y = position.y;
				transform.Rotation = glm::degrees(b2Rot_GetAngle(b2Body_GetRotation(rigid.RuntimeBody)));
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

		Project::SetActiveRoom(this);

		auto& scriptEngine = Project::GetScriptEngine();

		m_Registry.view<ScriptContainer>().each([&](auto entity, ScriptContainer& container) {
			for (auto& script : container)
			{
				if (scriptEngine->IsValidScript(script.ClassID))
				{
					script.Instance = scriptEngine->CreateInstance(script.ClassID);
					script.Instance.Invoke("OnCreate");
				}
				else
					STY_CORE_WARN("Object '{}' has an invalid script id ({})", (uint32_t)entity, (uint64_t)script.ClassID);
			}
		});

		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = { 0.0f, -m_Gravity };
		m_PhysicsWorld = b2CreateWorld(&worldDef);
		
		m_Registry.view<Transform, RigidBodyComponent>().each([&](auto entity, Transform& transform, RigidBodyComponent& rigid) {
			Object temp{ entity, this };
			
			if (!temp.HasComponent<ColliderComponent>())
			{
				STY_CORE_WARN("Cannot apply physics to object ({}) without collider", (uint32_t)temp);
				return;
			}

			ColliderComponent& collider = temp.GetComponent<ColliderComponent>();

			b2BodyDef bodyDef = b2DefaultBodyDef();
			bodyDef.type = (b2BodyType)rigid.Type;
			bodyDef.position = { transform.Position.x, transform.Position.y };
			bodyDef.fixedRotation = rigid.FixedRotation;
			bodyDef.rotation = b2MakeRot(glm::radians(transform.Rotation));
		
			rigid.RuntimeBody = b2CreateBody(m_PhysicsWorld, &bodyDef);

			b2ShapeDef shapeDef = b2DefaultShapeDef();
			shapeDef.density = 1.0f;
			b2Polygon polygonShape = b2MakeBox(collider.Dimensions.x, collider.Dimensions.y);

			b2CreatePolygonShape(rigid.RuntimeBody, &shapeDef, &polygonShape);
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

		Project::SetActiveRoom(nullptr);

		auto& scriptEngine = Project::GetScriptEngine();

		m_Registry.view<ScriptContainer>().each([&](auto entity, ScriptContainer& container) {
			for (auto& script : container)
			{
				script.Instance.Invoke("OnDestroy");
				scriptEngine->DestroyInstance(script.Instance);
			}
		});

		b2DestroyWorld(m_PhysicsWorld);

		m_RoomState = RoomState::Editor;
	}

	void Room::CopyTo(Ref<Room>& room)
	{
		room->Handle = Handle;
		room->m_BackgroundColour = m_BackgroundColour;

		for (auto entity : m_Registry.storage<entt::entity>())
		{
			Object temp{ entity, this };

			Object::CopyInto(temp, room->CreateObject());
		}
	}

	Object Room::CreateObject()
	{
		Object object{ m_Registry.create(), this };
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