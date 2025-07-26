#pragma once

#include <Strype.hpp>

namespace Strype {

	class RuntimeLayer : public Layer
	{
	public:
		RuntimeLayer(const std::filesystem::path& path)
			: m_ProjectPath(path) {}
		~RuntimeLayer();

		void OnAttach();
		void OnUpdate(float ts) override;

		void OpenProject(const std::filesystem::path& path);
	private:
		Ref<Room> m_Room;
		std::filesystem::path m_ProjectPath;
	};

}