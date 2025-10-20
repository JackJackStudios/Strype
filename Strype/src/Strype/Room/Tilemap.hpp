#pragma once

#include "Strype/Project/Asset.hpp"
#include "Strype/Renderer/Sprite.hpp"

namespace Strype {

	class Tilemap : public Asset
	{
	public:
		static AssetType GetStaticType() { return AssetType::Tilemap; }
		virtual AssetType GetType() const override { return GetStaticType(); }
	private:
		glm::uvec2 m_TileSize;
		AssetHandle m_AtlasSprite;

		friend Ref<Asset> load_tilemap_asset(const std::filesystem::path&);
		friend void save_tilemap_asset(Ref<Asset>, const std::filesystem::path&);
	};

}