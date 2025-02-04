#include <Strype.h>
#include <Strype/Core/EntryPoint.h>

namespace Strype {

	class ExampleLayer : public Layer
	{
	public:
		ExampleLayer()
		{
		}

		void OnAttach() override
		{
		}

		void OnUpdate(Timestep ts) override
		{
		}

		virtual void OnImGuiRender() override
		{
			ImGui::Begin("Test");
			ImGui::Text("Hello, Strype-Editor!");
			ImGui::End();
		}

		void OnEvent(Event& e) override
		{
		}
	};

	class Sandbox : public Application
	{
	public:
		Sandbox(const AppConfig& config)
			: Application(config)
		{
			PushLayer(new ExampleLayer());
		}

		~Sandbox()
		{

		}

	};

	Application* CreateApplication()
	{
		AppConfig config;
		config.DockspaceEnabled = true;

		return new Sandbox(config);
	}

}