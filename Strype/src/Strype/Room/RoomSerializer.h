#pragma once

#include "Strype/Asset/AssetSerializer.h"

#include "Room.h"

namespace Strype {

	class RoomSerializer : public AssetSerializer
	{
	public:
		virtual Ref<Asset> LoadAsset(const std::filesystem::path& path);

		virtual void SaveAsset(Ref<Asset> asset, const std::filesystem::path& path);
	};

}