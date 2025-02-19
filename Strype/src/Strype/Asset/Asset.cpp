#include "stypch.h"
#include "Asset.h"

namespace Strype::Utils {

	std::string_view AssetTypeToString(AssetType type)
	{
		switch (type)
		{
		case AssetType::None:    return "AssetType::None";
		case AssetType::Room:    return "AssetType::Room";
		case AssetType::Texture: return "AssetType::Texture";
		}

		return "AssetType::<Invalid>";
	}

	AssetType AssetTypeFromString(std::string_view assetType)
	{
		if (assetType == "AssetType::None")    return AssetType::None;
		if (assetType == "AssetType::Room")    return AssetType::Room;
		if (assetType == "AssetType::Texture") return AssetType::Texture;

		return AssetType::None;
	}

}