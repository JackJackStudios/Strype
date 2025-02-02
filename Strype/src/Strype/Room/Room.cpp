#include "stypch.h"
#include "Room.h"

#include "Strype/Renderer/Renderer.h"
#include "Strype/Room/Object.h"
#include "Strype/Room/Components.h"

#include <glm/glm.hpp>

namespace Strype {

	Room::Room()
		: m_CameraController(1280.0f / 720.0f)
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

	void Room::OnUpdate(Timestep ts)
	{
		m_CameraController.OnUpdate(ts);

		Renderer::BeginScene(m_CameraController.GetCamera());

		m_Registry.view<Transform, SpriteRenderer>().each([](auto entity, Transform& trans, SpriteRenderer& sprite) {
			Renderer::DrawRotatedQuad(trans.Position, trans.Scale, trans.Rotation, sprite.Texture, sprite.Colour);
		});

		Renderer::EndScene();
	}
	void Room::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
	}
}