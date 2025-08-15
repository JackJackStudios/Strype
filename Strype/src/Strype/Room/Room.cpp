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
		Renderer::GetCurrent()->BeginRoom(m_Camera);

		Renderer::GetCurrent()->DrawQuad({ 0.0f, 0.0f, 0.0f }, { m_Width, m_Height }, 0.0f, glm::make_vec4(m_BackgroundColour));

		if (m_RoomState == RoomState::Editor)
		{
			int controlX = Input::IsKeyDown(KeyCode::D) - Input::IsKeyDown(KeyCode::A);
			int controlY = Input::IsKeyDown(KeyCode::S) - Input::IsKeyDown(KeyCode::W);
			m_Camera.Position.x += controlX * m_CameraSpeed * ts;
			m_Camera.Position.y += controlY * m_CameraSpeed * ts;

			m_Camera.UpdateMatrix();
		}

		auto& scriptEngine = Project::GetScriptEngine();

		for (auto& instance : m_Objects)
		{
			auto sprite = Project::GetAsset<Sprite>(Project::GetAsset<Object>(instance.ObjectHandle)->TextureHandle);

			Renderer::GetCurrent()->DrawQuad(
				glm::make_vec3(instance.Position),
				instance.Scale,
				instance.Rotation,
				instance.Colour,
				sprite,
				instance.CurrentFrame,

				Buffer((float)(instance.m_Handle+1))
			);

			if (m_RoomState == RoomState::Runtime)
			{
				instance.CSharp->InvokeMethod("OnUpdate", ts);
				instance.CurrentFrame += sprite->GetFrameDelta();
				
				if (instance.CurrentFrame > sprite->FrameCount())
					instance.CurrentFrame = instance.CurrentFrame - (float)sprite->FrameCount();
			}
		}

		if (m_RoomState == RoomState::Runtime) b2World_Step(m_Physics, ts, 4);
		Renderer::GetCurrent()->EndRoom();
	}

	void Room::StartRuntime()
	{
		if (m_RoomState == RoomState::Runtime)
		{
			STY_CORE_WARN("Cannot start room ({}) twice!", FilePath.filename());
			return;
		}

		Project::SetActiveRoom(this);
		STY_CORE_TRACE("Staring room \"{}\" ", FilePath.stem());

		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = { 0.0f, -m_Gravity };
		m_Physics = b2CreateWorld(&worldDef);

		auto& scriptEngine = Project::GetScriptEngine();

		for (auto& instance : m_Objects)
		{
			auto object = Project::GetAsset<Object>(instance.ObjectHandle);

			b2BodyDef bodyDef = b2DefaultBodyDef();
			bodyDef.type = (b2BodyType)object->BodyType;
			bodyDef.position = { instance.Position.x, instance.Position.y };
			instance.RuntimeBody = b2CreateBody(m_Physics, &bodyDef);

			b2ShapeDef shapeDef = b2DefaultShapeDef(); shapeDef.density = 1.0f;
			b2Polygon polygonShape = b2MakeBox(instance.Scale.x, instance.Scale.y);
			b2CreatePolygonShape(instance.RuntimeBody, &shapeDef, &polygonShape);

			ScriptID scriptID = Project::GetAsset<Object>(instance.ObjectHandle)->ClassID;

			if (scriptEngine->IsValidScript(scriptID))
			{
				instance.CSharp = scriptEngine->CreateInstance(scriptID);
				instance.CSharp->SetFieldValueRaw("ID",     &instance.m_Handle);
				instance.CSharp->SetFieldValueRaw("Handle", &instance.ObjectHandle);

				instance.CSharp->InvokeMethod("OnCreate");
			}
			else
			{
				STY_CORE_WARN("Object '{}' has an invalid script id ({})", instance.m_Handle, scriptID);
			}
		}

		m_RoomState = RoomState::Runtime;
	}

	void Room::StopRuntime()
	{
		if (m_RoomState == RoomState::Editor)
		{
			STY_CORE_WARN("Cannot stop room ({}) twice", FilePath.filename());
			return;
		}

		auto& scriptEngine = Project::GetScriptEngine();
		Project::SetActiveRoom(nullptr);

		for (auto& instance : m_Objects)
		{
			instance.CSharp->InvokeMethod("OnDestroy");
			scriptEngine->DestroyInstance(instance.CSharp);
		}

		b2DestroyWorld(m_Physics);

		m_RoomState = RoomState::Editor;
	}

	Ref<Room> Room::CopyTo()
	{
		Ref<Room> room = CreateRef<Room>();

		room->Handle = Handle;
		room->FilePath = FilePath;

		room->m_Width = m_Width;
		room->m_Height = m_Height;
		room->m_BackgroundColour = m_BackgroundColour;

		room->m_Objects = m_Objects;

		return room;
	}

	InstanceID Room::InstantiatePrefab(AssetHandle prefab)
	{
		return m_Objects.emplace_back(UUID32(m_Objects.size()), prefab, this).m_Handle;
	}

	void Room::DestroyInstance(InstanceID obj)
	{
		m_Objects.erase(std::find_if(m_Objects.begin(), m_Objects.end(), [&](const RoomInstance& other) -> bool { return other.GetHandle() == obj; }));
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

	void Room::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_ZoomLevel += e.GetOffset().y * 0.25f;
		m_ZoomLevel = glm::max(m_ZoomLevel, 0.25f);
		m_Camera.SetZoomLevel(m_ZoomLevel);
	}

}