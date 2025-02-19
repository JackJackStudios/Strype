#include "stypch.h"
#include "TextureImporter.h"

#include "Strype/Project/Project.h"

namespace Strype {

	Ref<Texture>TextureImporter::ImportTexture(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadTexture(Project::GetProjectDirectory() / metadata.FilePath);
	}

	Ref<Texture>TextureImporter::LoadTexture(const std::filesystem::path& path)
	{
		//TODO: Move stbi_load to this file
		return Texture::Create(path.string());
	}

}