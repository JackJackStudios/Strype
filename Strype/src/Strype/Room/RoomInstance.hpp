#pragma once

#include "Strype/Audio/Audio.hpp"

#include <glm/glm.hpp>
#include <Coral/ManagedObject.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <box2d/box2d.h>

namespace Strype {

	using InstanceID = UUID32;
	class Room;

	class RoomInstance
	{
	public:
		RoomInstance(InstanceID id, AssetHandle derived, Room* owner)
			: m_Handle(id), ObjectHandle(derived), m_Owner(owner)
		{
		}

		glm::mat4 GetTransform() const
		{
			return glm::translate(glm::mat4(1.0f), { Position.x, Position.y, 0.0f })
				* glm::rotate(glm::mat4(1.0f), glm::radians(Rotation), { 0.0f, 0.0f, 1.0f })
				* glm::scale(glm::mat4(1.0f), { Scale.x, Scale.y, 1.0f });
		}

		InstanceID GetHandle() const { return m_Handle; }
		operator uint32_t() const { return GetHandle(); }

		bool operator!=(const InstanceID& other) { return m_Handle != other; }
		bool operator==(const InstanceID& other) { return m_Handle == other; }
	public:
		glm::vec4 Colour{ 1.0f, 1.0f, 1.0f, 1.0f };

		glm::vec2 Position{ 0.0f, 0.0f };
		glm::vec2 Scale{ 1.0f, 1.0f };
		float Rotation = 0.0f;

		AssetHandle ObjectHandle;
		Coral::ManagedObject* CSharp;
		b2BodyId RuntimeBody;
		//Ref<AudioSource> Emitter;
	private:
		InstanceID m_Handle = 0;
		Room* m_Owner = nullptr;

		friend class Room;
	};

}