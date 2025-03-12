#include "stypch.h"
#include "RoomSerializer.h"

#include "Object.h"
#include "Prefab.h"
#include "Components.h"

#include "Strype/Utils/YamlHelpers.h"
#include "Strype/Project/Project.h"

#include <fstream>

namespace Strype {

	RoomSerializer::RoomSerializer(const Ref<Room>& room)
		: m_Room(room)
	{
	}

	static void SerializeObject(Object obj, YAML::Emitter& out)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Object" << YAML::Value << (uint32_t)obj;

		if (obj.HasComponent<PrefabComponent>())
		{
			out << YAML::Key << "PrefabComponent";
			out << YAML::BeginMap;

			PrefabComponent& p = obj.GetComponent<PrefabComponent>();
			out << YAML::Key << "PrefabPath" << YAML::Value << Project::GetFilePath(p.Handle);

			out << YAML::EndMap;
		}

		if (obj.HasComponent<Transform>())
		{
			out << YAML::Key << "Transform";
			out << YAML::BeginMap;

			Transform& t = obj.GetComponent<Transform>();
			out << YAML::Key << "Position" << YAML::Value << t.Position;
			out << YAML::Key << "Scale" << YAML::Value << t.Scale;
			out << YAML::Key << "Rotation" << YAML::Value << t.Rotation;

			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void RoomSerializer::Serialize(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Room" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "Objects" << YAML::Value << YAML::BeginSeq;

		auto view = m_Room->m_Registry.view<entt::entity>();
		for (entt::entity id : view)
		{
			Object obj{ id, m_Room.get() };

			SerializeObject(obj, out);
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void RoomSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		std::ifstream fstream(filepath);

		STY_CORE_ASSERT(fstream.is_open(), "Error opening file");

		std::stringstream stream;
		stream << fstream.rdbuf();

		YAML::Node data = YAML::Load(stream.str())["Room"];

		STY_CORE_ASSERT(data, "Could not load room")
		STY_CORE_ASSERT(data["Objects"], "Could not load room")

		STY_CORE_TRACE("Deserializing room '{0}'", filepath.stem().string());
		
		m_Room->Clear();
		m_Room->m_Name = filepath.stem().string();

		YAML::Node objects = data["Objects"];
		for (auto obj : objects)
		{
			uint32_t id = obj["Object"].as<uint32_t>();

			STY_CORE_ASSERT(obj["PrefabComponent"], "Object ({0}) is not a valid Strype Object", id);

			STY_CORE_TRACE("Deserialized object with entt ID ({0})", id);

			YAML::Node prefab = obj["PrefabComponent"];
			const std::filesystem::path& path = prefab["PrefabPath"].as<std::filesystem::path>();

			if (!path.empty())
			{
				AssetHandle handle = Project::ImportAsset(path);
				Object newobj = Object::Copy(Project::GetAsset<Prefab>(handle)->GetObject(), m_Room);
				newobj.AddComponent<PrefabComponent>(handle);

				YAML::Node transform = obj["Transform"];
				if (transform)
				{
					Transform& tc = newobj.GetComponent<Transform>();

					tc.Position = transform["Position"].as<glm::vec2>();
					tc.Scale = transform["Scale"].as<glm::vec2>();
					tc.Rotation = transform["Rotation"].as<float>();
				}
			}
		}
	}

}