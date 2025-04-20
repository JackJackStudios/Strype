#include "stypch.h"
#include "Asset.h"

namespace Strype::Utils {

	std::string_view AssetTypeToString(AssetType type)
	{
		switch (type)
		{
		case AssetType::None:    return "AssetType::None";
		case AssetType::Prefab:  return "AssetType::Prefab";
		case AssetType::Room:    return "AssetType::Room";
		case AssetType::Sprite: return "AssetType::Sprite";
		case AssetType::AudioFile: return "AssetType::AudioFile";
		}

		return "AssetType::<Invalid>";
	}

	AssetType AssetTypeFromString(std::string_view assetType)
	{
		if (assetType == "AssetType::None")    return AssetType::None;
		if (assetType == "AssetType::Prefab")  return AssetType::Prefab;
		if (assetType == "AssetType::Room")    return AssetType::Room;
		if (assetType == "AssetType::Sprite") return AssetType::Sprite;
		if (assetType == "AssetType::AudioFile") return AssetType::AudioFile;

		return AssetType::None;
	}

}