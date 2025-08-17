#pragma once

#include "Asset.hpp"

#include "Strype/Renderer/Sprite.hpp"
#include "Strype/Audio/Audio.hpp"
#include "Strype/Room/Object.hpp"
#include "Strype/Room/Room.hpp"
#include "Strype/Script/ScriptEngine.hpp"

namespace Strype {

	class AssetSerializer
	{
	public:
		virtual Ref<Asset> LoadAsset(const std::filesystem::path& path) = 0;

		virtual void SaveAsset(Ref<Asset> asset, const std::filesystem::path& path) = 0;
	};

	class SpriteSerializer : public AssetSerializer
	{
	public:
		virtual Ref<Asset> LoadAsset(const std::filesystem::path& path);

		virtual void SaveAsset(Ref<Asset> asset, const std::filesystem::path& path) {};
	};

	class RoomSerializer : public AssetSerializer
	{
	public:
		virtual Ref<Asset> LoadAsset(const std::filesystem::path& path);

		virtual void SaveAsset(Ref<Asset> asset, const std::filesystem::path& path);
	};

	class AudioFileSerializer : public AssetSerializer
	{
	public:
		virtual Ref<Asset> LoadAsset(const std::filesystem::path& path);

		virtual void SaveAsset(Ref<Asset> asset, const std::filesystem::path& path) {};
	};

	class ObjectSerializer : public AssetSerializer
	{
	public:
		virtual Ref<Asset> LoadAsset(const std::filesystem::path& path);

		virtual void SaveAsset(Ref<Asset> asset, const std::filesystem::path& path);
	};

	class Script : public Asset
	{
	public:
		Script() = default;
		Script(ScriptID id)
			: m_ScriptID(id) 
		{
		}

		static AssetType GetStaticType() { return AssetType::Script; }
		virtual AssetType GetType() const override { return GetStaticType(); }

		operator ScriptID() { return m_ScriptID; }
		operator const ScriptID() const { return m_ScriptID; }

		ScriptID GetID() const { return m_ScriptID; }
	private:
		ScriptID m_ScriptID = 0;
	};

	class ScriptSerializer : public AssetSerializer
	{
	public:
		virtual Ref<Asset> LoadAsset(const std::filesystem::path& path);

		virtual void SaveAsset(Ref<Asset> asset, const std::filesystem::path& path);
	};

}