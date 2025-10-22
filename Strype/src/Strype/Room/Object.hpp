#pragma once

namespace Strype {

	enum class CollisionShape
	{
		None = 0, Rect, Circle
	};
	
	struct AABB
	{
		glm::vec2 Position;  // center
		glm::vec2 HalfSize;
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
		std::vector<UUID> Scripts; // UUID = ScriptID

		CollisionShape ShapeType = CollisionShape::None;
		AABB CollisionBox;
	};

}