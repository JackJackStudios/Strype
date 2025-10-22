#include "stypch.hpp"

#include "Strype/Project/AssetManager.hpp"
#include "Strype/Project/Project.hpp"

#include "Strype/Audio/Audio.hpp"
#include "Strype/Renderer/Sprite.hpp"
#include "Strype/Renderer/Font.hpp"
#include "Strype/Script/ScriptAsset.hpp"
#include "Strype/Room/Room.hpp"
#include "Strype/Room/Object.hpp"

#include "Strype/Utils/YamlHelpers.hpp"
#include <yaml-cpp/yaml.h>

#include <stb_image.h>
#include <regex>

#define ASSET_IMPORTER_FUNC(type, func)                                   \
    Ref<Asset> func(const std::filesystem::path&);                        \
    struct func##_registrar {                                             \
        func##_registrar() {                                              \
            s_AssetImportersMap[type] = func;                             \
        }                                                                 \
    };                                                                    \
    static inline func##_registrar s_##func##_registrar;                  \
    Ref<Asset> func

#define ASSET_EXPORTER_FUNC(type, func)                                    \
    void func(Ref<Asset>, const std::filesystem::path&);                   \
    struct func##_exporter_registrar {                                     \
        func##_exporter_registrar() {                                      \
            s_AssetExportersMap[type] = func;                              \
        }                                                                  \
    };                                                                     \
    static inline func##_exporter_registrar s_##func##_exporter_registrar; \
    void func


namespace Strype {

    inline static std::unordered_map<AssetType, AssetImporterFunc> s_AssetImportersMap;
    inline static std::unordered_map<AssetType, AssetExporterFunc> s_AssetExportersMap;

    //////////////////////////////////////////
    // LOADING ASSETS ////////////////////////
    //////////////////////////////////////////

    ASSET_IMPORTER_FUNC(AssetType::Sprite, load_sprite_asset)(const std::filesystem::path& filepath)
	{
        int frameCount = 1;
        int width, height, channels;

        stbi_set_flip_vertically_on_load(false);
        stbi_uc* data = stbi_load(filepath.string().c_str(), &width, &height, &channels, 0);
        if (!data) return nullptr;

        AGI::TextureSpecification textureSpec;
        textureSpec.Size = { width, height };
        textureSpec.Format = AGI::Utils::ChannelsToImageFormat(channels);

        AGI::Texture texture = Renderer::GetCurrent()->GetContext()->CreateTexture(textureSpec);
        texture->SetData(data, width * height * channels);

        std::regex pattern(".strip(\\d+)");
        std::smatch match;

        std::string pathStr = filepath.string();
        if (std::regex_search(pathStr, match, pattern))
            frameCount = std::stoi(match[1].str());

        return CreateRef<Sprite>(texture, frameCount);
	}

    ASSET_IMPORTER_FUNC(AssetType::AudioFile, load_audiofile_asset)(const std::filesystem::path& path)
    {
        ma_decoder decoder;
        ma_result result = ma_decoder_init_file(path.string().c_str(), nullptr, &decoder);
        if (result != MA_SUCCESS) return nullptr;

        return CreateRef<AudioFile>(decoder);
    }

    ASSET_IMPORTER_FUNC(AssetType::Script, load_script_asset)(const std::filesystem::path& path)
    {
        auto scriptEngine = Project::GetScriptEngine();
        ScriptID script = scriptEngine->GetIDByName(path.stem().string());
        if (!scriptEngine->IsValidScript(script))
        {
            STY_LOG_WARN("Script", "{} doesn't have corrosponding class in C# binaries", path.filename());
            return nullptr;
        }

        return CreateRef<Script>(script);
    }

    ASSET_IMPORTER_FUNC(AssetType::Object, load_object_asset)(const std::filesystem::path& path)
    {
        YAML::Node root = YAML::LoadFile(path.string())["Object"];
        if (!root) return nullptr;

        Ref<Object> object = CreateRef<Object>();
        Ref<AssetManager> manager = Project::GetAssetManager();
        Ref<ScriptEngine> scriptEngine = Project::GetScriptEngine();

        object->TextureHandle = manager->ImportAsset(root["SpritePath"].as<std::filesystem::path>());
        if (!manager->IsAssetLoaded(object->TextureHandle)) return nullptr;

        for (const auto& node : root["Scripts"])
        {
            std::string name = node.as<std::string>();
            ScriptID script = scriptEngine->GetIDByName(name);

            if (!scriptEngine->IsValidScript(script))
            {
                STY_LOG_WARN("\"{}\" doesnt exist in C# binaries", name);
                continue;
            }

            object->Scripts.emplace_back(script);
        }

        if (YAML::Node collision = root["Collision"])
        {
            object->ShapeType = magic_enum::enum_cast<CollisionShape>(collision["Shape"].as<std::string_view>()).value();
            object->CollisionBox.HalfSize = collision["Size"].as<glm::vec2>() * 0.5f;
            object->CollisionBox.Position = collision["Origin"].as<glm::vec2>() + object->CollisionBox.HalfSize;
        }

        return object;
    }

    ASSET_IMPORTER_FUNC(AssetType::Room, load_room_asset)(const std::filesystem::path& path)
    {
        YAML::Node data = YAML::LoadFile(path.string())["Room"];
        if (!data) return nullptr;

        Ref<Room> room = CreateRef<Room>();
        room->m_Size = data["Size"].as<glm::vec2>();
        room->m_Gravity = data["Gravity"].as<float>();
        room->m_BackgroundColour = data["BackgroundColour"].as<glm::vec3>();

        YAML::Node tilemap = data["Tilemap"];
        room->m_MainTilemap.AtlasHandle = Project::GetAssetManager()->ImportAsset(tilemap["SpritePath"].as<std::filesystem::path>());
        room->m_MainTilemap.TileSize = tilemap["TileSize"].as<glm::vec2>();

        YAML::Node objects = data["Objects"];
        for (auto obj : objects)
        {
            AssetHandle handle = Project::GetAssetManager()->ImportAsset(obj["ObjectPath"].as<std::filesystem::path>());
            if (!Project::GetAssetManager()->IsAssetLoaded(handle))
                continue;

            RoomInstance instance;
            instance.ObjectHandle = handle;
            instance.Position = obj["Position"].as<glm::vec2>();
            instance.Scale = obj["Scale"].as<glm::vec2>();
            instance.Rotation = obj["Rotation"].as<float>();
            room->CreateInstance(instance);
        }

        return room;
    }

    ASSET_IMPORTER_FUNC(AssetType::Font, load_font_asset)(const std::filesystem::path& path)
    {
        int pixelHeight = 48;
        std::vector<CharsetRange> charset = { LatinCharacters };

        FT_Face face;
        FT_New_Face(Renderer::GetCurrent()->GetFreetypeLib(), path.string().c_str(), 0, &face);
        FT_Set_Pixel_Sizes(face, 0, pixelHeight);

        return CreateRef<Font>(face, charset);
    }

    //////////////////////////////////////////
    // SAVING ASSETS /////////////////////////
    //////////////////////////////////////////

    ASSET_EXPORTER_FUNC(AssetType::Room, save_room_asset)(Ref<Asset> asset, const std::filesystem::path& filepath)
    {
        Room* room = (Room*)asset.get();

        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Room" << YAML::BeginMap;

        out << YAML::Key << "Size" << YAML::Value << room->m_Size;
        out << YAML::Key << "Gravity" << YAML::Value << room->m_Gravity;
        out << YAML::Key << "BackgroundColour" << YAML::Value << room->m_BackgroundColour;

        {
            out << YAML::Key << "Tilemap" << YAML::BeginMap;

            out << YAML::Key << "SpritePath" << Project::GetAssetManager()->GetFilePath(room->m_MainTilemap.AtlasHandle);
            out << YAML::Key << "TileSize" << room->m_MainTilemap.TileSize;

            out << YAML::EndMap;
        }

        {
            out << YAML::Key << "Objects" << YAML::BeginSeq;

            for (const auto& obj : room->m_Objects)
            {
                out  << YAML::BeginMap;

                out << YAML::Key << "ObjectPath" << YAML::Value << Project::GetAssetManager()->GetFilePath(obj.ObjectHandle);
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

    ASSET_EXPORTER_FUNC(AssetType::Script, save_script_asset)(Ref<Asset> asset, const std::filesystem::path& filepath)
    {
        Script* script = (Script*)asset.get();
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

    ASSET_EXPORTER_FUNC(AssetType::Object, save_object_asset)(Ref<Asset> asset, const std::filesystem::path& filepath)
    {
        Object* object = (Object*)asset.get();

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

        {
            out << YAML::Key << "Collision" << YAML::BeginMap;

            out << YAML::Key << "Shape" << YAML::Value << object->ShapeType;
            out << YAML::Key << "Size" << YAML::Value << object->CollisionBox.HalfSize * 2.0f;
            out << YAML::Key << "Origin" << YAML::Value << object->CollisionBox.Position - object->CollisionBox.HalfSize;

            out << YAML::EndMap;
        }

        out << YAML::EndMap;
        out << YAML::EndMap;
        Utils::WriteFile(filepath, out.c_str());
    }

};