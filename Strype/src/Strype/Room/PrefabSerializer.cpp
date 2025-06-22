#include "stypch.hpp"
#include "PrefabSerializer.hpp"

#include "Strype/Utils/YamlHelpers.hpp"
#include "Strype/Project/Project.hpp"

#include <fstream>
#include <magic_enum/magic_enum.hpp>

namespace Strype {

	void PrefabSerializer::SaveAsset(Ref<Asset> asset, const std::filesystem::path& path)
	{
		//HACK: Assume asset is prefab 
		Prefab* prefab = (Prefab*)asset.get();

		YAML::Emitter out;
		{
			ScopedMap root(out);

			{
				ScopedMap prefabMap(out, "Prefab");

				auto& scriptEngine = Project::GetScriptEngine();

				out << YAML::Key << "TexturePath" << YAML::Value
					<< (prefab->TextureHandle ? Project::GetFilePath(prefab->TextureHandle) : "");
				out << YAML::Key << "ClassID" << YAML::Value << prefab->ClassID;
			}
		}

		std::ofstream fout(path);
		fout << out.c_str();
	}

	Ref<Asset> PrefabSerializer::LoadAsset(const std::filesystem::path& path)
	{
		Ref<Prefab> prefab = CreateRef<Prefab>();
		YAML::Node data = YAML::LoadFile(path.string())["Prefab"];
		
		const std::filesystem::path& texturePath = data["TexturePath"].as<std::filesystem::path>();
		
		if (!texturePath.empty())
		{
			AssetHandle handle = Project::ImportAsset(texturePath);
			prefab->TextureHandle = handle;
		}

		auto& scriptEngine = Project::GetScriptEngine();
		prefab->ClassID = data["ClassID"].as<ScriptID>();
		
		return prefab;
	}

}


