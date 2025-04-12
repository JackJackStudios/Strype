#pragma once

#include "Asset.h"

namespace Strype {

	class AssetSerializer
	{
	public:
		virtual Ref<Asset> LoadAsset(const std::filesystem::path& path) = 0;

		virtual void SaveAsset(Ref<Asset> asset, const std::filesystem::path& path) = 0;
	};

}