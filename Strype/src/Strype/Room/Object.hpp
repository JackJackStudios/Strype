#pragma once

#include "RoomInstance.hpp"

#include <box2d/box2d.h>

namespace Strype {

	enum class PhysicsType
	{
		Static = b2_staticBody,
		Kinematic = b2_kinematicBody,
		Dynamic = b2_dynamicBody,
	};
	
	class Object : public Asset
	{
	public:
		Object() = default;
		Object(const Object& other) = default;

		static AssetType GetStaticType() { return AssetType::Object; }
		virtual AssetType GetType() const override { return GetStaticType(); }
	public:
		AssetHandle TextureHandle = 0;
		PhysicsType BodyType = PhysicsType::Dynamic;
		std::vector<UUID> Scripts;
	};

}