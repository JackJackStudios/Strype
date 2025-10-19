#include "stypch.hpp"
#include "Strype/Room/Room.hpp"

#include "Strype/Project/Project.hpp"
#include "Strype/Room/Object.hpp"
#include "Strype/Script/ScriptEngine.hpp"

#include <glm/glm.hpp>

namespace Strype {

	void Room::OnUpdate(float ts)
	{
		if (!IsActive()) return;
		for (auto& instance : m_Objects)
		{
			Ref<Sprite> sprite = Project::GetAsset<Sprite>(Project::GetAsset<Object>(instance.ObjectHandle)->TextureHandle);

			for (const auto& csharp : instance.CSharpObjects)
				csharp->InvokeMethod("OnUpdate", ts);

			instance.CurrentFrame += sprite->GetFrameDelta(ts) * instance.AnimationSpeed;
			instance.CurrentFrame = fmod(instance.CurrentFrame, (float)sprite->GetFrameCount());
		}
	}

	void Room::OnRender(Renderer* renderer)
	{
		if (m_Camera.GetSize().x == 0 || m_Camera.GetSize().y == 0)
			STY_CORE_WARN("Cannot resize camera to {}", m_Camera.GetSize());

		m_Camera.UpdateMatrix();

		renderer->BeginRoom(m_Camera);
		renderer->DrawSprite({ 0.0f, 0.0f, 0.0f }, m_Size, 0.0f, { m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, 1.0f });

		for (auto& instance : m_Objects)
		{
			// NOTE: GetAsset() returns nullptr when object doesn't have a sprite. 
			if (AssetHandle handle = Project::GetAsset<Object>(instance.ObjectHandle)->TextureHandle)
			{
				Ref<Sprite> sprite = Project::GetAsset<Sprite>(handle);
				renderer->DrawSprite({ instance.Position.x, instance.Position.y, 0.0f },
					instance.Scale, instance.Rotation, instance.Colour, sprite, instance.CurrentFrame);
			}
		}

		renderer->EndRoom();
	}

	void Room::ToggleRuntime(bool toggle)
	{
		RoomState newstate = (toggle ? RoomState::Runtime : RoomState::Editor);
		if (m_RoomState == newstate) return;

		if (newstate == RoomState::Runtime)
		{
			// Start runtime
			Project::SetActiveRoom(this);
			m_Camera.SetZoomLevel(1.0f);

			Ref<ScriptEngine> scriptEngine = Project::GetScriptEngine();
			for (auto& handle : m_Reverse)
			{
				RoomInstance& instance = GetInstance(handle);
				Ref<Object> object = Project::GetAsset<Object>(instance.ObjectHandle);
				instance.CSharpObjects.reserve(object->Scripts.size());

				for (const auto& scriptID : object->Scripts)
				{
					if (!scriptEngine->IsValidScript(scriptID))
					{
						STY_CORE_WARN("Object \"{}\" has an invalid script id ({})", object->Name, scriptID);
						continue;
					}

					auto& csharp = instance.CSharpObjects.emplace_back(scriptEngine->CreateInstance(scriptID));
					csharp->SetFieldValueRaw("Handle", &instance.ObjectHandle);
					csharp->SetFieldValueRaw("ID", &handle);

					csharp->InvokeMethod("OnCreate");
				}
			}
		}
		else if (newstate == RoomState::Editor)
		{
			if (m_RoomState == RoomState::Runtime)
			{
				// End runtime
				Project::SetActiveRoom(nullptr);

				Ref<ScriptEngine> scriptEngine = Project::GetScriptEngine();
				for (auto& instance : m_Objects)
				{
					for (auto& csharp : instance.CSharpObjects)
					{
						csharp->InvokeMethod("OnDestroy");
						scriptEngine->DestroyInstance(csharp);
					}

					instance.CSharpObjects.clear();
				}
			}
		}
		
		m_RoomState = newstate;
	}

}