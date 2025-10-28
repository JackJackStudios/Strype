#include "stypch.hpp"
#include "Strype/Room/Room.hpp"

#include "Strype/Project/Project.hpp"
#include "Strype/Room/Object.hpp"

#include <glm/glm.hpp>

namespace Strype {

	glm::vec2 AABB_Resolve(const RoomInstance& instance1, const RoomInstance& instance2)
	{
		AABB a = Project::GetAsset<Object>(instance1.ObjectHandle)->CollisionBox;
		AABB b = Project::GetAsset<Object>(instance2.ObjectHandle)->CollisionBox;
		a.Position += instance1.Position;
		b.Position += instance2.Position;

		float dx = b.Position.x - a.Position.x;
		float px = (b.HalfSize.x + a.HalfSize.x) - glm::abs(dx);

		float dy = b.Position.y - a.Position.y;
		float py = (b.HalfSize.y + a.HalfSize.y) - glm::abs(dy);

		if (px <= 0 || py <= 0)
			return { 0, 0 };

		if (px < py) 
			return { dx < 0 ? -px : px, 0 };
		else
			return { 0, dy < 0 ? -py : py };
	}

	void Room::OnUpdate(float ts)
	{
		if (!IsActive()) return;

		for (auto& [name, manager] : m_Managers)
		{
			manager.InvokeMethod("OnUpdate", ts);
		}

		for (size_t i = 0; i < m_Objects.size(); ++i)
		{
			auto& instance = m_Objects[i];
			Ref<Object> object = Project::GetAsset<Object>(instance.ObjectHandle);
			Ref<Sprite> sprite = Project::GetAsset<Sprite>(object->TextureHandle);

			for (const auto& csharp : instance.CSharpObjects)
				csharp->InvokeMethod("OnUpdate", ts);

			instance.CurrentFrame += sprite->GetFrameDelta(ts) * instance.AnimationSpeed;
			instance.CurrentFrame = fmod(instance.CurrentFrame, (float)sprite->GetFrameCount());

			if (object->ShapeType != CollisionShape::None)
			{
				for (size_t j = i + 1; j < m_Objects.size(); ++j)
				{
					auto& compare = m_Objects[j];

					glm::vec2 correction = AABB_Resolve(instance, compare);
					if (correction.x != 0 || correction.y != 0)
					{
						instance.Position -= correction * 0.5f;
						compare.Position += correction * 0.5f;
					}
				}
			}
		}
	}

	void Room::OnRender(Renderer* renderer)
	{
		if (m_Camera.GetSize().x == 0 || m_Camera.GetSize().y == 0)
			STY_LOG_WARN("Room", "Cannot resize camera to {}", m_Camera.GetSize());

		m_Camera.UpdateMatrix();

		renderer->BeginRoom(m_Camera);
		renderer->DrawRect({ 0.0f, 0.0f, 0.0f }, m_Size, { m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, 1.0f });

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

	void Room::OnEvent(Event& e)
	{

	}

	void Room::ToggleRuntime(bool toggle, Ref<Room> oldState)
	{
		RoomState newstate = (toggle ? RoomState::Runtime : RoomState::Editor);
		if (m_RoomState == newstate) return;

		if (newstate == RoomState::Runtime)
		{
			// Start runtime
			Project::SetActiveRoom(this);
			m_Camera.SetZoomLevel(1.0f);

			Ref<ScriptEngine> scriptEngine = Project::GetScriptEngine();

			if (oldState == nullptr)
			{
				scriptEngine->GetAllChildren("Strype.Manager", [&](Coral::Type* child)
				{
					std::string name = (std::string)child->GetFullName();
					Coral::ManagedObject instance = child->CreateInstance();

					STY_LOG_TRACE("Room", "Detected manager class: \"{}\" ", name);
					m_Managers[name] = instance;

					instance.InvokeMethod("OnCreate");
				});
			}
			else
			{
				m_Managers = oldState->m_Managers;
			}

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