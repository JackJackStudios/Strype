#include "stypch.hpp"
#include "Room.hpp"

#include "Object.hpp"
#include "RoomInstance.hpp"

#include "Strype/Utils/YamlHelpers.hpp"
#include "Strype/Project/Project.hpp"
#include "Strype/Project/AssetSerializer.hpp"

#include <fstream>

namespace Strype {

	void RoomSerializer::SaveAsset(Ref<Asset> asset, const std::filesystem::path& path)
	{
		//HACK: Assume asset is room 
		Room* room = (Room*)asset.get();
		if (room->m_RoomState == RoomState::Runtime)
		{
			return;
		}

		YAML::Emitter out;
		{
			ScopedMap root(out);

			{
				ScopedMap roomMap(out, "Room");

				out << YAML::Key << "Width" << YAML::Value << room->m_Width;
				out << YAML::Key << "Height" << YAML::Value << room->m_Height;
				out << YAML::Key << "BackgroundColour" << YAML::Value << room->m_BackgroundColour;
				
				{
					ScopedSeq objectsSeq(out, "Objects");

					for (const auto& obj : room->m_Objects) 
					{
						ScopedMap objMap(out);

						out << YAML::Key << "Instance ID" << YAML::Value << obj.GetHandle();
						out << YAML::Key << "ObjectPath" << YAML::Value << Project::GetFilePath(obj.ObjectHandle);
						out << YAML::Key << "Colour" << YAML::Value << obj.Colour;

						out << YAML::Key << "Position" << YAML::Value << obj.Position;
						out << YAML::Key << "Scale" << YAML::Value << obj.Scale;
						out << YAML::Key << "Rotation" << YAML::Value << obj.Rotation;
					}
				}
			}
		}

		std::ofstream fout(path);
		fout << out.c_str();
	}

	Ref<Asset> RoomSerializer::LoadAsset(const std::filesystem::path& path)
	{
		YAML::Node data = YAML::LoadFile(path.string())["Room"];
		if (!data) return nullptr;

		Ref<Room> room = CreateRef<Room>();
		room->m_Width = data["Width"].as<uint64_t>();
		room->m_Height = data["Height"].as<uint64_t>();
		room->m_BackgroundColour = data["BackgroundColour"].as<glm::vec3>();

		YAML::Node objects = data["Objects"];
		for (auto obj : objects)
		{
			AssetHandle handle = Project::ImportAsset(obj["ObjectPath"].as<std::filesystem::path>());
			if (!Project::IsAssetLoaded(handle))
				continue;

			RoomInstance& newobj = room->GetObject(room->CreateInstance(handle));
			newobj.ObjectHandle = handle;

			newobj.Position = obj["Position"].as<glm::vec2>();
			newobj.Scale = obj["Scale"].as<glm::vec2>();
			newobj.Rotation = obj["Rotation"].as<float>();
			newobj.Colour = obj["Colour"].as<glm::vec4>();
		}
	
		return room;
	}

}