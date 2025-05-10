#pragma once

#include "Strype/Core/UUID.h"

namespace Strype {

	using AssetHandle = UUID;

	enum class AssetType : uint16_t
	{
		None = 0,
		Prefab,
		Sprite,
		Room,
		AudioFile,
	};

	class Asset
	{
	public:
		AssetHandle Handle;

		virtual AssetType GetType() const = 0;
	};

	struct AssetMetadata
	{
		Ref<Asset> asset;
		AssetType Type = AssetType::None;
		std::filesystem::path FilePath;

		void reset() { asset.reset(); }

		operator bool() const { return Type != AssetType::None; }
	};

}