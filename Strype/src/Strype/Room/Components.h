#pragma once

#include "Strype/Script/ScriptEngine.h"
#include "Strype/Script/CSharpObject.h"

#include <agi.h>
#include <glm/glm.hpp>

namespace Strype {

	struct PrefabComponent
	{
		AssetHandle Handle;

		PrefabComponent() = default;
		PrefabComponent(const PrefabComponent&) = default;
		PrefabComponent(AssetHandle handle)
			: Handle(handle) {}
	};

	struct Transform
	{
		glm::vec2 Position;
		glm::vec2 Scale;
		float Rotation;

		Transform() = default;
		Transform(const Transform&) = default;
		Transform(const glm::vec2& position, const glm::vec2& scale = glm::vec2(1.0f), float rotation = 0)
			: Position(position), Scale(scale), Rotation(rotation) {}
	};

	struct ScriptComponent
	{
		ScriptID ClassID;
		CSharpObject Instance;
	};

	struct SpriteRenderer
	{
		glm::vec4 Colour;
		AssetHandle Texture;

		SpriteRenderer() = default;
		SpriteRenderer(const SpriteRenderer&) = default;
		SpriteRenderer(AssetHandle texture, const glm::vec4& colour = glm::vec4(1.0f))
			: Texture(texture), Colour(colour)  {}
	};

}