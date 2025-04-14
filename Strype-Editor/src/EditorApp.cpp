#include <Strype.h>
#include <Strype/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Strype {

	class Editor : public Application
	{
	public:
		Editor(const AppConfig& config)
			: Application(config)
		{
			PushLayer(new EditorLayer());
		}

		~Editor()
		{

		}

	};

	Application* CreateApplication()
	{
		AppConfig config;
		config.StartupFrames = 10;
		config.DockspaceEnabled = true;

		return new Editor(config);
	}

}