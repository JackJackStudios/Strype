#include "stypch.hpp"
#include "Strype/Project/AssetSerializer.hpp"

#include "Strype/Utils/YamlHelpers.hpp"
#include "Strype/Project/Project.hpp"

#include "Object.hpp"

#include <yaml-cpp/yaml.h>

namespace Strype {

	void ObjectSerializer::SaveAsset(Ref<Asset> asset, const std::filesystem::path& path)
	{
		//HACK: Assume asset is room 
		Object* object = (Object*)asset.get();

		YAML::Emitter out;
		out << YAML::BeginMap;

		{
			ScopedMap root(out, "Object");
			out << YAML::Key << "SpritePath" << YAML::Value << Project::GetFilePath(object->TextureHandle);

            {
                ScopedSeq scriptsSeq(out, "Scripts", true);

                for (const auto& scriptID : object->Scripts)
                    out << Project::GetScriptEngine()->GetScriptName(scriptID);
            }
		}

		std::ofstream fout(path);
		out << YAML::EndMap;
		fout << out.c_str();
	}

    Ref<Asset> ObjectSerializer::LoadAsset(const std::filesystem::path& path)
    {
        YAML::Node root = YAML::LoadFile(path.string())["Object"];
        if (!root) return nullptr;

        Ref<Object> object = CreateRef<Object>();
        object->TextureHandle = Project::ImportAsset(root["SpritePath"].as<std::filesystem::path>());

        for (const auto& node : root["Scripts"])
        {
            std::string name = node.as<std::string>();

            auto& scriptEngine = Project::GetScriptEngine();
            ScriptID script = scriptEngine->GetIDByName(name);

            if (!scriptEngine->IsValidScript(script))
            {
                STY_CORE_WARN("\"{}\" doesnt exist in C# binaries", name);
                continue;
            }

            object->Scripts.emplace_back(script);
        }

        return object;
    }

};