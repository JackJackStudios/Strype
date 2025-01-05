#include "Strype.h"
#include "Strype/Core/EntryPoint.h"

class ExampleLayer : public Strype::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		if (Strype::Input::IsKeyPressed(Strype::Key::Tab))
			STY_TRACE("Tab key is pressed (poll)!");
	}

	void OnEvent(Strype::Event& event) override
	{
	}
};

class Sandbox : public Strype::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Strype::ImGuiLayer());
	}

	~Sandbox()
	{

	}
};

Strype::Application* Strype::CreateApplication()
{
	return new Sandbox();
}