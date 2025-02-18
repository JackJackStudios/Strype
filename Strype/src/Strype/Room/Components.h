#pragma once

#include "Strype/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Strype {

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {
		}

		operator std::string () { return Tag; }
		operator const char* () { return Tag.c_str();  }
	};

	struct Transform
	{
		glm::vec2 Position;
		glm::vec2 Scale;
		float Rotation;

		Transform(const Transform&) = default;
		Transform()
			: Position(0.0f), Scale(1.0f), Rotation(0) {}
		Transform(const glm::vec2& position, const glm::vec2& scale = glm::vec2(1.0f), float rotation = 0)
			: Position(position), Scale(scale), Rotation(rotation) {}
	};

	struct SpriteRenderer
	{
		glm::vec4 Colour;
		Ref<Texture> Texture;

		SpriteRenderer(const SpriteRenderer&) = default;
		SpriteRenderer()
			: Colour(1.0f), Texture(nullptr) {}
		SpriteRenderer(const Ref<Strype::Texture>& texture, const glm::vec4& colour = glm::vec4(1.0f))
			: Texture(texture), Colour(colour)  {}
	};

}