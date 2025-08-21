#pragma once

#include "Strype.hpp"

namespace Strype {

	class RuntimeSession : public Session
	{
	public:
		~RuntimeSession();
		RuntimeSession(const std::filesystem::path& path)
			: m_ProjectPath(path) 
		{
			WindowProps.Title = "Strype-Runtime";
		}

		void OnAttach();
		void OnUpdate(float ts) override;

		void OpenProject(const std::filesystem::path& path);
	private:
		Ref<Room> m_Room;
		std::filesystem::path m_ProjectPath;
	};

}