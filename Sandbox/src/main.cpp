#include "Strype.h"
#include "Strype/Core/EntryPoint.h"

class Sandbox : public Strype::Application
{
public:
	Sandbox()
	{
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