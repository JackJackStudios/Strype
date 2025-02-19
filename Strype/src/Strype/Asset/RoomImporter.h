#pragma once

#include "Asset.h"

#include "Strype/Room/Room.h"

namespace Strype {

	class RoomImporter
	{
	public:
		static Ref<Room> ImportRoom(AssetHandle handle, const AssetMetadata& metadata);

		static Ref<Room> LoadRoom(const std::filesystem::path& path);

		static void SaveRoom(Ref<Room> room, const std::filesystem::path& path);
	};

}