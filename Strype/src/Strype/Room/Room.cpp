#include "stypch.h"
#include "Room.h"

#include "Strype/Renderer/Renderer.h"
#include "Strype/Renderer/Sprite.h"

#include "Strype/Room/Object.h"
#include "Strype/Room/Components.h"
#include "Strype/Project/Project.h"

#include <glm/glm.hpp>

namespace Strype {

	Room::Room()
	{
	}

	Room::~Room()
	{
	}

	Object Room::CreateObject(std::string name, glm::vec3 position)
	{
		Object object{ m_Registry.create(), this };
		object.AddComponent<Transform>(position);
		return object;
	}

	void Room::DestroyObject(Object entity)
	{
		m_Registry.destroy(entity);
	}

	void Room::Clear()
	{
		m_Registry.clear();
	}

	void Room::OnUpdate(Timestep ts, Camera cam)
	{
		Renderer::BeginRoom(cam);

		m_Registry.view<Transform, SpriteRenderer>().each([](auto entity, Transform& trans, SpriteRenderer& sprite) {
			if (sprite.Texture)
			{
				Renderer::DrawRotatedQuad(trans.Position, trans.Scale, trans.Rotation, Project::GetAsset<Sprite>(sprite.Texture)->Texture, sprite.Colour);
			}
			else
			{
				Renderer::DrawRotatedQuad(trans.Position, trans.Scale, trans.Rotation, sprite.Colour);
			}
		});

		Renderer::EndRoom();
	}

}