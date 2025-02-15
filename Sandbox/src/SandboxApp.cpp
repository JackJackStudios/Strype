#include <Strype.h>
#include <Strype/Core/EntryPoint.h>

namespace Strype {

	class ExampleLayer : public Layer
	{
	public:
		ExampleLayer()
		{
		}

		void OnAttach()
		{
		}

		void OnUpdate(Timestep ts) override
		{
			if (Input::IsKeyPressed(KeyCode::A))
				STY_CORE_INFO("A key pressed!");
			
			if (Input::IsKeyHeld(KeyCode::A))
				STY_CORE_INFO("A key held!");
			
			if (Input::IsKeyReleased(KeyCode::A))
				STY_CORE_INFO("A key released!");
		}

		virtual void OnImGuiRender() override
		{
		}

		void OnEvent(Event& e) override
		{
		}

	private:
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
		config.DockspaceEnabled = false;

		return new Sandbox(config);
	}

}