#pragma once

#include "Room.h"

namespace Strype {

	class RoomSerializer
	{
	public:
		RoomSerializer(const Ref<Room>& room);

		void Serialize(const std::string& filepath);
		void Deserialize(const std::string& filepath);
	private:
		Ref<Room> m_Room;
	};

}