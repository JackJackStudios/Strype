#pragma once

#include "Asset.hpp"

#include "Strype/Renderer/Sprite.hpp"
#include "Strype/Audio/Audio.hpp"

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

	class AudioFileSerializer : public AssetSerializer
	{
	public:
		virtual Ref<Asset> LoadAsset(const std::filesystem::path& path);

		virtual void SaveAsset(Ref<Asset> asset, const std::filesystem::path& path) {};
	};

}