#pragma once

#include "Strype/Script/ScriptEngine.hpp"
#include "Strype/Project/Asset.hpp"

namespace Strype {

	class Script : public Asset
	{
	public:
		Script() = default;
		Script(ScriptID id)
			: m_ScriptID(id)
		{
		}

		static AssetType GetStaticType() { return AssetType::Script; }
		virtual AssetType GetType() const override { return GetStaticType(); }

		operator ScriptID() { return m_ScriptID; }
		operator const ScriptID() const { return m_ScriptID; }

		ScriptID GetID() const { return m_ScriptID; }
	private:
		ScriptID m_ScriptID = 0;
	};

};