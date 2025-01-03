#include "Strype.h"
#include "Strype/Core/EntryPoint.h"

class Sandbox : public Strype::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

Strype::Application* Strype::CreateApplication()
{
	return new Sandbox();
}