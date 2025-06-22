#pragma once

#include "Strype/Script/ScriptEngine.hpp"

#include "Object.hpp"

namespace Strype {

	class Prefab : public Asset
	{
	public:
		Prefab() = default;
		Prefab(const Prefab& other) = default;

		static AssetType GetStaticType() { return AssetType::Prefab; }
		virtual AssetType GetType() const override { return GetStaticType(); }
	public:
		AssetHandle TextureHandle;
		ScriptID ClassID;
	};

}