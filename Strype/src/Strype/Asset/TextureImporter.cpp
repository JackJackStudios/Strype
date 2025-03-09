#include "stypch.h"
#include "TextureImporter.h"

#include "Strype/Project/Project.h"

namespace Strype {

	Ref<Sprite> TextureImporter::ImportTexture(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadTexture(Project::GetProjectDirectory() / metadata.FilePath);
	}

	Ref<Sprite> TextureImporter::LoadTexture(const std::filesystem::path& path)
	{
		//TODO: Move stbi_load to this file
		return CreateRef<Sprite>(path);
	}

}