#include <Strype.h>
#include <Strype/Core/EntryPoint.h>

#include "RuntimeLayer.h"

namespace Strype {

	class Runtime : public Application
	{
	public:
		Runtime(const AppConfig& config)
			: Application(config)
		{
			PushLayer(new RuntimeLayer());
		}

		~Runtime()
		{

		}

	};

	Application* CreateApplication()
	{
		AppConfig config;
		config.StartupFrames = 10;
		config.DockspaceEnabled = false;

		return new Runtime(config);
	}

}