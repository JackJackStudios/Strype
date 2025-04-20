#pragma once

#include "Strype/Audio/Audio.h"

#include "Strype/Asset/Asset.h"
#include "Strype/Asset/AssetSerializer.h"

#include "Strype/Project/Project.h"

namespace Strype {

	class AudioFileSerializer : public AssetSerializer
	{
	public:
		virtual Ref<Asset> LoadAsset(const std::filesystem::path& path)
		{
			return CreateRef<AudioFile>(Project::GetProjectDirectory() / path);
		}

		virtual void SaveAsset(Ref<Asset> asset, const std::filesystem::path& path) {};
	};

}