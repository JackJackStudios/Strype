#include "stypch.hpp"
#include "RoomSerializer.hpp"

#include "Object.hpp"
#include "RoomInstance.hpp"

#include "Strype/Utils/YamlHelpers.hpp"
#include "Strype/Project/Project.hpp"

#include <fstream>

namespace Strype {

	void RoomSerializer::SaveAsset(Ref<Asset> asset, const std::filesystem::path& path)
	{
		//HACK: Assume asset is prefab 
		Room* room = (Room*)asset.get();

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

						out << YAML::Key << "Object" << YAML::Value << static_cast<uint32_t>(obj);
						out << YAML::Key << "PrefabPath" << YAML::Value << Project::GetFilePath(obj.PrefabHandle);
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
		Ref<Room> room = CreateRef<Room>();
		YAML::Node data = YAML::LoadFile(path.string())["Room"];

		STY_CORE_VERIFY(data, "Could not load room")
		STY_CORE_VERIFY(data["Objects"], "Could not load room");
		STY_CORE_VERIFY(data["BackgroundColour"], "Could not load room");

		YAML::Node width = data["Width"];
		YAML::Node height = data["Height"];
		if (width && height)
		{
			room->m_Width = data["Width"].as<uint64_t>();
			room->m_Height = data["Height"].as<uint64_t>();
		}

		room->m_BackgroundColour = data["BackgroundColour"].as<glm::vec3>();

		room->Clear();

		YAML::Node objects = data["Objects"];
		for (auto obj : objects)
		{
			InstanceID id = obj["Object"].as<InstanceID>();

			const std::filesystem::path& path = obj["PrefabPath"].as<std::filesystem::path>();
			if (!std::filesystem::exists(Project::GetProjectDirectory() / path))
			{
				STY_CORE_WARN("Could not find specifed path: \"{}\" ", path);
				continue;
			}

			AssetHandle handle = Project::ImportAsset(path);

			RoomInstance& newobj = room->GetObject(room->InstantiatePrefab(handle));
			newobj.PrefabHandle = handle;

			newobj.Position = obj["Position"].as<glm::vec2>();
			newobj.Scale = obj["Scale"].as<glm::vec2>();
			newobj.Rotation = obj["Rotation"].as<float>();
			newobj.Colour = obj["Colour"].as<glm::vec4>();
		}
	
		return room;
	}

}