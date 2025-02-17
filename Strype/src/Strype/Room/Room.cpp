#include "stypch.h"
#include "Room.h"

#include "Strype/Renderer/Renderer.h"
#include "Strype/Room/Object.h"
#include "Strype/Room/Components.h"

#include <glm/glm.hpp>

namespace Strype {

	Room::Room()
	{
	}

	Room::~Room()
	{
	}

	Object Room::CreateObject(std::string name)
	{
		Object object{ m_Registry.create(), this };
		object.AddComponent<TagComponent>(name);
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
		Renderer::BeginScene(cam);

		m_Registry.view<Transform, SpriteRenderer>().each([](auto entity, Transform& trans, SpriteRenderer& sprite) {
			if (sprite.Texture.get() == nullptr)
			{
				Renderer::DrawRotatedQuad(trans.Position, trans.Scale, trans.Rotation, sprite.Colour);
			}
			else
			{
				Renderer::DrawRotatedQuad(trans.Position, trans.Scale, trans.Rotation, sprite.Texture, sprite.Colour);
			}
		});

		Renderer::EndScene();
	}

}