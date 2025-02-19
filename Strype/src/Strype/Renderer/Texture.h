#pragma once

#include "Strype/Asset/Asset.h"
#include "Strype/Core/Base.h"

#include <string>

namespace Strype {

	class Texture : public Asset
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData(const Buffer& data) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool operator==(const Texture& other) const = 0;

		virtual AssetType GetType() const override { return AssetType::Texture; }

		static Ref<Texture> Create(uint32_t width, uint32_t height, uint16_t channels);
		static Ref<Texture> Create(const std::string& path);
	};

}