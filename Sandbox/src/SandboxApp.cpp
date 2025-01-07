#include "Strype.h"
#include "Strype/Core/EntryPoint.h"

class ExampleLayer : public Strype::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate(Strype::Timestep) override
	{
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello, world");
		ImGui::End();
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
	}

	~Sandbox()
	{

	}

};

Strype::Application* Strype::CreateApplication()
{
	return new Sandbox();
}