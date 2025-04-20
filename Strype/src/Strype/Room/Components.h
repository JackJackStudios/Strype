#pragma once

#include "Strype/Script/ScriptEngine.h"
#include "Strype/Script/CSharpObject.h"

#include <agi.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>

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

	struct SpriteRenderer
	{
		glm::vec4 Colour{ 1.0f, 1.0f, 1.0f, 1.0f };
		AssetHandle Texture;

		SpriteRenderer() = default;
		SpriteRenderer(const SpriteRenderer&) = default;
		SpriteRenderer(AssetHandle texture, const glm::vec4& colour = glm::vec4(1.0f))
			: Texture(texture), Colour(colour) {
		}
	};

	struct ScriptComponent
	{
		ScriptID ClassID;
		CSharpObject Instance;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};

	enum class BodyType 
	{ 
		Static, 
		Dynamic, 
		Kinematic 
	};

	struct RigidBodyComponent
	{
		BodyType Type = BodyType::Static;
		bool FixedRotation = true;

		b2BodyId RuntimeBody = (b2BodyId)-1; //Undefined

		RigidBodyComponent() = default;
		RigidBodyComponent(const RigidBodyComponent&) = default;
		RigidBodyComponent(BodyType type, bool fixedRotaion = false)
			: Type(type), FixedRotation(fixedRotaion) {
		}
	};

}