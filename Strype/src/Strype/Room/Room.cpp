#include "stypch.hpp"
#include "Strype/Room/Room.hpp"

#include "Strype/Project/Project.hpp"
#include "Strype/Room/Object.hpp"

#include <glm/glm.hpp>

namespace Strype {

	void Room::OnUpdate(float ts)
	{
		for (auto& instance : m_Objects)
		{
			if (m_RoomState != RoomState::Runtime) break;
			Ref<Sprite> sprite = Project::GetAsset<Sprite>(Project::GetAsset<Object>(instance.ObjectHandle)->TextureHandle);

			for (const auto& csharp : instance.CSharpObjects)
				csharp->InvokeMethod("OnUpdate", ts);

			instance.CurrentFrame += sprite->GetFrameDelta();
			instance.CurrentFrame = fmod(instance.CurrentFrame, (float)sprite->GetFrameCount());

			const auto& position = b2Body_GetPosition(instance.RuntimeBody);
			instance.Position.x = position.x;
			instance.Position.y = position.y;
			instance.Rotation = glm::degrees(b2Rot_GetAngle(b2Body_GetRotation(instance.RuntimeBody)));
		}
	}

	void Room::OnRender(Scope<Renderer>& renderer)
	{
		renderer->BeginRoom(m_Camera);
		renderer->DrawSprite({ 0.0f, 0.0f, 0.0f }, m_Size, 0.0f, { m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, 0.0f });

		m_Camera.UpdateMatrix();
		for (auto& instance : m_Objects)
		{
			Ref<Sprite> sprite = Project::GetAsset<Sprite>(Project::GetAsset<Object>(instance.ObjectHandle)->TextureHandle);
			renderer->DrawSprite({ instance.Position.x, instance.Position.y, 0.0f },
			                     instance.Scale, instance.Rotation, instance.Colour, sprite, instance.CurrentFrame);
		}

		renderer->EndRoom();
	}

	void Room::ToggleRuntime(bool toggle)
	{

	}

	void Room::TogglePause(bool toggle)
	{

	}

}