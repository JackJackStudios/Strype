#pragma once

#include "Prefab.h"

namespace Strype {

	class PrefabSerializer
	{
	public:
		PrefabSerializer(const Ref<Prefab>& prefab);

		void Serialize(const std::filesystem::path& filepath);
		void Deserialize(const std::filesystem::path& filepath);
	private:
		Ref<Prefab> m_Prefab;
	};

}