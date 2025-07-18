#pragma once

#include "Strype/Core/UUID.hpp"

namespace Strype {

	using AssetHandle = UUID;

	enum class AssetType : uint16_t
	{
		None = 0, Object, Sprite, Room, AudioFile
	};

	class Asset
	{
	public:
		AssetHandle Handle;

		virtual AssetType GetType() const = 0;
	};

	struct AssetMetadata
	{
		Ref<Asset> Asset;
		std::filesystem::path FilePath;

		void reset() { Asset.reset(); }
		operator bool() const { return Asset->GetType() != AssetType::None; }
	};

}