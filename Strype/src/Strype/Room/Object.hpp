#pragma once

#include "RoomInstance.hpp"

namespace Strype {

	class Object : public Asset
	{
	public:
		Object() = default;
		Object(const Object& other) = default;

		static AssetType GetStaticType() { return AssetType::Object; }
		virtual AssetType GetType() const override { return GetStaticType(); }
	public:
		AssetHandle TextureHandle;
		UUID ClassID;
	};

}