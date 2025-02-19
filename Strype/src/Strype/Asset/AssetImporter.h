#pragma once

#include "Asset.h"

namespace Strype {

	class AssetImporter
	{
	public:
		static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
	};

}