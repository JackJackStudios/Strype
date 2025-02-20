#pragma once

#include "Strype/Core/UUID.h"

namespace Strype {

	using AssetHandle = UUID;

	enum class AssetType : uint16_t
	{
		None = 0,
		Texture,
		Room,
	};

	namespace Utils {

		std::string_view AssetTypeToString(AssetType type);
		AssetType AssetTypeFromString(std::string_view assetType);

	}

	struct AssetMetadata
	{
		AssetHandle Handle;
		AssetType Type = AssetType::None;
		std::filesystem::path FilePath;

		operator bool() const { return Type != AssetType::None; }
	};

	class Asset
	{
	public:
		AssetHandle Handle;

		virtual AssetType GetType() const = 0;
	};

}