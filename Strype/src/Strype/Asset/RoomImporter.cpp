#include "stypch.h"
#include "RoomImporter.h"

#include "Strype/Project/Project.h"
#include "Strype/Room/RoomSerializer.h"

namespace Strype {

	Ref<Room> RoomImporter::ImportRoom(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadRoom(Project::GetProjectDirectory() / metadata.FilePath);
	}

	Ref<Room> RoomImporter::LoadRoom(const std::filesystem::path& path)
	{
		Ref<Room> room = CreateRef<Room>();
		RoomSerializer serializer(room);
		serializer.Deserialize(path);
		return room;
	}

	void RoomImporter::SaveRoom(Ref<Room> room, const std::filesystem::path& path)
	{
		RoomSerializer serializer(room);
		serializer.Serialize(Project::GetProjectDirectory() / path);
	}

}