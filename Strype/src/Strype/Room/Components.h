#pragma once

#include "Strype/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Strype {

	struct Transform
	{
		glm::vec2 Scale{ 1.0f };
		glm::vec3 Position;
		float Rotation;

		Transform() = default;
		Transform(const Transform&) = default;
		Transform(const glm::vec3& position, const glm::vec2& scale = glm::vec2(1.0f), float rotation = 0)
			: Position(position), Scale(scale), Rotation(rotation) {}
	};

	struct SpriteRenderer
	{
		glm::vec4 Colour{ 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture> Texture;

		SpriteRenderer(const SpriteRenderer&) = default;
		SpriteRenderer(const Ref<Strype::Texture>& texture, const glm::vec4& colour = glm::vec4(1.0f))
			: Texture(texture), Colour(colour)  {}
	};

}