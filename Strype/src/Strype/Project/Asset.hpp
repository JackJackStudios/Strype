#pragma once

#include "Strype/Core/UUID.hpp"

namespace Strype {

	using AssetHandle = UUID;

	enum class AssetType : uint16_t
	{
		None = 0, Object, Sprite, Room, AudioFile, Script
	};

	class Asset
	{
	public:
		AssetHandle Handle;
		std::string Name;

		virtual AssetType GetType() const = 0;
	};

	struct AssetMetadata
	{
		AssetHandle Handle;
		std::filesystem::path Filepath;
	};

};