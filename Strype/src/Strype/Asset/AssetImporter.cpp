#include "stypch.h"
#include "AssetImporter.h"

#include "TextureImporter.h"
#include "RoomImporter.h"

namespace Strype {

	using AssetImportFunc = std::function<Ref<Asset>(AssetHandle, const AssetMetadata&)>;
	static std::map<AssetType, AssetImportFunc> s_AssetImportFunctions = {
		{ AssetType::Texture, TextureImporter::ImportTexture },
		{ AssetType::Room, RoomImporter::ImportRoom },
	};

	Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		if (s_AssetImportFunctions.find(metadata.Type) == s_AssetImportFunctions.end())
		{
			STY_CORE_ERROR("No importer available for asset type: {}", (uint16_t)metadata.Type);
			return nullptr;
		}

		return s_AssetImportFunctions.at(metadata.Type)(handle, metadata);
	}

}