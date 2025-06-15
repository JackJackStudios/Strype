#pragma once

#include <Strype.hpp>

namespace Strype {

	constexpr float CAMERA_WIDTH = 1280.0f;
	constexpr float CAMERA_HEIGHT = 720.0f;
	constexpr float ASPECT_RATIO = CAMERA_WIDTH / CAMERA_HEIGHT;

	class RuntimeLayer : public Layer
	{
	public:
		RuntimeLayer(const std::filesystem::path& path);
		~RuntimeLayer();

		void OnUpdate(float ts) override;

		void OpenProject(const std::filesystem::path& path);
	private:
		Ref<Room> m_Room;
	};

}