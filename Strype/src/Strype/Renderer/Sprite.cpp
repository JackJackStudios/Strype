#include "stypch.h"
#include "Sprite.h"

namespace Strype {

	Sprite::Sprite(Ref<AGI::Texture> texture)
		: Texture(texture)
	{
	}

	Sprite::Sprite(const std::filesystem::path& filepath)
		: Texture(AGI::Texture::Create(filepath.string()))
	{
	}

}

