#include "stypch.h"
#include "PrefabSerializer.h"

#include "Components.h"

#include "Strype/Utils/YamlHelpers.h"
#include "Strype/Project/Project.h"

#include <fstream>
#include <magic_enum/magic_enum.hpp>

namespace Strype {

	void PrefabSerializer::SaveAsset(Ref<Asset> asset, const std::filesystem::path& path)
	{
		//HACK: Assume asset is prefab 
		Prefab* prefab = (Prefab*)asset.get();

		YAML::Emitter out;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Prefab" << YAML::Value << YAML::BeginMap;

			if (prefab->HasComponent<SpriteRenderer>())
			{
				out << YAML::Key << "SpriteRenderer" << YAML::Value;
				out << YAML::BeginMap;

				SpriteRenderer& s = prefab->GetComponent<SpriteRenderer>();
				out << YAML::Key << "Colour" << YAML::Value << s.Colour;
				out << YAML::Key << "TexturePath" << YAML::Value << (s.Texture ? Project::GetFilePath(s.Texture) : "");

				out << YAML::EndMap;
			}

			if (prefab->HasComponent<ScriptComponent>())
			{
				out << YAML::Key << "ScriptComponent" << YAML::Value;
				out << YAML::BeginMap;

				ScriptComponent& sc = prefab->GetComponent<ScriptComponent>();
				out << YAML::Key << "ClassName" << YAML::Value << Project::GetScriptEngine()->GetScriptName(sc.ClassID);

				out << YAML::EndMap;
			}

			if (prefab->HasComponent<RigidBodyComponent>())
			{
				out << YAML::Key << "RigidBodyComponent" << YAML::Value;
				out << YAML::BeginMap;

				RigidBodyComponent& rbc = prefab->GetComponent<RigidBodyComponent>();
				out << YAML::Key << "Type" << YAML::Value << magic_enum::enum_name(rbc.Type);
				out << YAML::Key << "FixedRotation" << YAML::Value << rbc.FixedRotation;

				out << YAML::EndMap;
			}

			if (prefab->HasComponent<ColliderComponent>())
			{
				out << YAML::Key << "ColliderComponent" << YAML::Value;
				out << YAML::BeginMap;

				ColliderComponent& cc = prefab->GetComponent<ColliderComponent>();
				out << YAML::Key << "Shape" << YAML::Value << magic_enum::enum_name(cc.Shape);
				out << YAML::Key << "Dimensions" << YAML::Value << cc.Dimensions;

				out << YAML::EndMap;
			}

			out << YAML::EndMap;
		}
		out << YAML::EndMap;
		std::ofstream fout(path);
		fout << out.c_str();
	}

	Ref<Asset> PrefabSerializer::LoadAsset(const std::filesystem::path& path)
	{
		Ref<Prefab> prefab = CreateRef<Prefab>();

		std::ifstream fstream(path);
		STY_CORE_VERIFY(fstream.is_open(), "Error opening file");

		std::stringstream stream;
		stream << fstream.rdbuf();

		YAML::Node data = YAML::Load(stream.str())["Prefab"];
		STY_CORE_VERIFY(data, "Could not load prefab")

		Object newobj = s_PrefabRoom->CreateObject();
			
		YAML::Node sprite = data["SpriteRenderer"];
		if (sprite)
		{
			SpriteRenderer& src = newobj.AddComponent<SpriteRenderer>();
			src.Colour = sprite["Colour"].as<glm::vec4>();
		
			const std::filesystem::path& path = sprite["TexturePath"].as<std::filesystem::path>();
		
			if (!path.empty())
			{
				AssetHandle handle = Project::ImportAsset(path);
				src.Texture = handle;
			}
		}

		YAML::Node script = data["ScriptComponent"];
		if (script)
		{
			ScriptComponent& sc = newobj.AddComponent<ScriptComponent>();
			sc.ClassID = Project::GetScriptEngine()->GetIDByName(script["ClassName"].as<std::string>());
		}

		YAML::Node rigid = data["RigidBodyComponent"];
		if (rigid)
		{
			RigidBodyComponent& rbc = newobj.AddComponent<RigidBodyComponent>();
			rbc.Type = magic_enum::enum_cast<BodyType>(rigid["Type"].as<std::string>()).value_or(BodyType::Static);
			rbc.FixedRotation = rigid["FixedRotation"].as<bool>();
		}

		YAML::Node collider = data["ColliderComponent"];
		if (collider)
		{
			ColliderComponent& cc = newobj.AddComponent<ColliderComponent>();
			cc.Shape = magic_enum::enum_cast<ColliderShape>(collider["Shape"].as<std::string>()).value_or(ColliderShape::Polygon);
			cc.Dimensions = collider["Dimensions"].as<glm::vec2>();
		}

		prefab->SetObject(newobj);
		return prefab;
	}

}


