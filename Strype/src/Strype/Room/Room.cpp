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
			STY_LOG_WARN("Room", "Cannot resize camera to {}", m_Camera.GetSize());

		m_Camera.UpdateMatrix();

		renderer->BeginRoom(m_Camera);
		renderer->DrawRect({ 0.0f, 0.0f, 0.0f }, m_Size, 0.0f, { m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, 1.0f });

		if (AssetHandle handle = m_MainTilemap.AtlasHandle)
		{
			Ref<Sprite> atlas = Project::GetAsset<Sprite>(handle);
			int tilesPerWidth = atlas->GetFrameSize().x / m_MainTilemap.TileSize.x;

			for (const auto& [position, index] : m_MainTilemap.PackedData)
			{
				if (index == 0)
				{
					m_MainTilemap.PackedData.erase(position);
					continue;
				}

				glm::vec2 gridPosition = position * m_MainTilemap.TileSize;
				int tileX = (index - 1) % tilesPerWidth;
				int tileY = (index - 1) / tilesPerWidth;

				renderer->DrawSprite({ gridPosition.x, gridPosition.y, 0.0f }, 
					{ 1.0f, 1.0f }, 0.0f, { 1.0f, 1.0f, 1.0f, 1.0f }, atlas, 0, SpriteAlign(HoriAlign::Left, VertAlign::Top), 
					Utils::BoxToTextureCoords(glm::vec2(tileX * m_MainTilemap.TileSize.x, tileY * m_MainTilemap.TileSize.y), m_MainTilemap.TileSize.x, m_MainTilemap.TileSize.y, atlas->GetFrameSize()));
			}
		}

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
						STY_LOG_WARN("Room", "Object \"{}\" has an invalid script id ({})", object->Name, scriptID);
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