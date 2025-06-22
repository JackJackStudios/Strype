#pragma once

#include "Strype/Script/CSharpObject.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Strype {

	struct Transform
	{
		glm::vec2 Position;
		glm::vec2 Scale;
		float Rotation;

		glm::mat4 GetTransform() const
		{
			return glm::translate(glm::mat4(1.0f), { Position.x, Position.y, 0.0f })
				* glm::rotate(glm::mat4(1.0f), glm::radians(Rotation), { 0.0f, 0.0f, 1.0f })
				* glm::scale(glm::mat4(1.0f), { Scale.x, Scale.y, 1.0f });
		}
	};

	using InstanceID = UUID32;
	class Room;

	class RoomInstance
	{
	public:
		RoomInstance(InstanceID id, AssetHandle prefab, Room* owner)
			: m_Handle(id), PrefabHandle(prefab), m_Owner(owner)
		{
		}

		InstanceID GetHandle() const { return m_Handle; }
		operator uint32_t() const { return GetHandle(); }

		bool operator!=(const InstanceID& other) { return m_Handle != other; }
		bool operator==(const InstanceID& other) { return m_Handle == other; }
	public:
		Transform Transform;
		glm::vec4 Colour{ 1.0f, 1.0f, 1.0f, 1.0f };
		CSharpObject Instance;
		AssetHandle PrefabHandle;
	private:
		InstanceID m_Handle = 0;
		Room* m_Owner = nullptr;

		friend class Room;
	};

}