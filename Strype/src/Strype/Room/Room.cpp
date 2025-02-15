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

	Object Room::CreateObject()
	{
		Object object{ m_Registry.create(), this };
		return object;
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