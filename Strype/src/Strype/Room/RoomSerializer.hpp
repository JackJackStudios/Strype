#pragma once

#include "Strype/Asset/AssetSerializer.hpp"

#include "Room.hpp"

namespace Strype {

	class RoomSerializer : public AssetSerializer
	{
	public:
		virtual Ref<Asset> LoadAsset(const std::filesystem::path& path);

		virtual void SaveAsset(Ref<Asset> asset, const std::filesystem::path& path);
	};

}