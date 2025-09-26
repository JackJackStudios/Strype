#include "stypch.hpp"

#include "Strype/Project/AssetManager.hpp"
#include "Strype/Project/Project.hpp"

#include "Strype/Audio/Audio.hpp"
#include "Strype/Renderer/Sprite.hpp"
#include "Strype/Script/ScriptAsset.hpp"
#include "Strype/Room/Room.hpp"
#include "Strype/Room/Object.hpp"

#include "Strype/Utils/YamlHelpers.hpp"
#include <yaml-cpp/yaml.h>

#include <stb_image.h>
#include <regex>

namespace Strype {

    //////////////////////////////////////////
    // LOADING ASSETS ////////////////////////
    //////////////////////////////////////////

    Ref<Asset> load_sprite_asset(const std::filesystem::path& filepath)
	{
        int frameCount = 1;
        int width, height, channels;

        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = stbi_load(filepath.string().c_str(), &width, &height, &channels, 0);
        if (!data) return nullptr;

        AGI::TextureSpecification textureSpec;
        textureSpec.Width = width;
        textureSpec.Height = height;
        textureSpec.Format = AGI::Utils::ChannelsToImageFormat(channels);

        textureSpec.Data = data;
        textureSpec.Datasize = width * height * channels;

        std::regex pattern(".strip(\\d+)");
        std::smatch match;

        std::string pathStr = filepath.string();
        if (std::regex_search(pathStr, match, pattern))
            frameCount = std::stoi(match[1].str());

        return CreateRef<Sprite>(textureSpec, frameCount);
	}

    Ref<Asset> load_audiofile_asset(const std::filesystem::path& path)
    {
        ma_decoder decoder;
        ma_result result = ma_decoder_init_file(path.string().c_str(), nullptr, &decoder);
        if (result != MA_SUCCESS) return nullptr;

        return CreateRef<AudioFile>(decoder);
    }

    Ref<Asset> load_script_asset(const std::filesystem::path& path)
    {
        auto& scriptEngine = Project::GetScriptEngine();
        ScriptID script = scriptEngine->GetIDByName(path.stem().string());
        if (!scriptEngine->IsValidScript(script))
        {
            STY_CORE_WARN("{} doesn't have corrosponding class in C# binaries", path.filename());
            return nullptr;
        }

        return CreateRef<Script>(script);
    }

    Ref<Asset> load_object_asset(const std::filesystem::path& path)
    {
        YAML::Node root = YAML::LoadFile(path.string())["Object"];
        if (!root) return nullptr;

        Ref<Object> object = CreateRef<Object>();

        auto filepath = Project::GetProjectDirectory() / root["SpritePath"].as<std::filesystem::path>();
        if (std::filesystem::exists(filepath) && filepath.has_filename())
        {
            object->TextureHandle = Project::GetAssetManager()->ImportAsset(filepath);
            if (!Project::GetAssetManager()->IsAssetLoaded(object->TextureHandle)) return nullptr;
        }

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

    Ref<Asset> load_room_asset(const std::filesystem::path& path)
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
            AssetHandle handle = Project::GetAssetManager()->ImportAsset(obj["ObjectPath"].as<std::filesystem::path>());
            if (!Project::GetAssetManager()->IsAssetLoaded(handle))
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

    //////////////////////////////////////////
    // SAVING ASSETS /////////////////////////
    //////////////////////////////////////////

    void save_room_asset(Ref<Room> room, const std::filesystem::path& filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Room" << YAML::BeginMap;

        out << YAML::Key << "Width" << YAML::Value << room->m_Width;
        out << YAML::Key << "Height" << YAML::Value << room->m_Height;
        out << YAML::Key << "BackgroundColour" << YAML::Value << room->m_BackgroundColour;

        {
            out << YAML::Key << "Objects" << YAML::BeginSeq;

            for (const auto& obj : room->m_Objects)
            {
                out  << YAML::BeginMap;

                out << YAML::Key << "Instance ID" << YAML::Value << obj.GetHandle();
                out << YAML::Key << "ObjectPath" << YAML::Value << Project::GetAssetManager()->GetFilePath(obj.ObjectHandle);
                out << YAML::Key << "Colour" << YAML::Value << obj.Colour;

                out << YAML::Key << "Position" << YAML::Value << obj.Position;
                out << YAML::Key << "Scale" << YAML::Value << obj.Scale;
                out << YAML::Key << "Rotation" << YAML::Value << obj.Rotation;

                out << YAML::EndMap;
            }

            out << YAML::EndSeq;
        }

        out << YAML::EndMap;
        out << YAML::EndMap;
        Utils::WriteFile(filepath, out.c_str());
    }

    void save_script_asset(Ref<Script> script, const std::filesystem::path& filepath)
    {
        if (script->GetID() != 0) return;
        std::string scriptTemplate = Utils::ReadFile("assets/ScriptTemplate.cs");

        size_t pos = 0;
        while ((pos = scriptTemplate.find("Template", pos)) != std::string::npos)
        {
            scriptTemplate.replace(pos, std::string("Template").length(), filepath.stem().string());
            pos += filepath.stem().string().length();
        }
    
        Utils::WriteFile(filepath, scriptTemplate);
    }

    void save_object_asset(Ref<Object> object, const std::filesystem::path& filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Object" << YAML::BeginMap;

        out << YAML::Key << "SpritePath"; 
        out << YAML::Value << (Project::GetAssetManager()->IsAssetLoaded(object->TextureHandle) ? Project::GetAssetManager()->GetFilePath(object->TextureHandle) : "");

        {
            out << YAML::Key << "Scripts" << YAML::Flow << YAML::BeginSeq;

            for (const auto& scriptID : object->Scripts)
                out << Project::GetScriptEngine()->GetScriptName(scriptID);

            out << YAML::EndSeq;
        }

        out << YAML::EndMap;
        out << YAML::EndMap;
        Utils::WriteFile(filepath, out.c_str());
    }

    static std::unordered_map<std::filesystem::path, std::function<Ref<Asset>(const std::filesystem::path&)>> s_AssetImportersMap = {
        { ".png",   load_sprite_asset },
        //{ ".jpg",   load_sprite_asset },
        //{ ".jpeg",  load_sprite_asset },
        //{ ".bmp",   load_sprite_asset },

        { ".wav",   load_audiofile_asset },
        //{ ".mp3",   load_audiofile_asset },
        //{ ".flac",  load_audiofile_asset },
        //{ ".ogg",   load_audiofile_asset },

        { ".cs",    load_script_asset },
    };

};