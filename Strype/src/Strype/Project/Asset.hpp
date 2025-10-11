#pragma once

#include "Strype/Core/UUID.hpp"

namespace Strype {

	using AssetHandle = UUID;

	enum class AssetType : uint16_t
	{
		None = 0, Object, Sprite, Room, AudioFile, Script, Font
	};

	class Asset
	{
	public:
		virtual ~Asset() = default;
		virtual AssetType GetType() const = 0;

		AssetHandle Handle;
		std::string Name;
	};

	struct AssetMetadata
	{
		AssetHandle Handle;
		std::filesystem::path Filepath;
	};

};