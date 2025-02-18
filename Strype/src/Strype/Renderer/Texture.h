#pragma once

#include <string>

#include "Strype/Core/Base.h"

namespace Strype {

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData(const Buffer& data) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool operator==(const Texture& other) const = 0;

		static Ref<Texture> Create(uint32_t width, uint32_t height, uint16_t channels);
		static Ref<Texture> Create(const std::string& path);
	};

}