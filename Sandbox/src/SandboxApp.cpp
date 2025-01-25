#include "Strype.h"
#include "Strype/Core/EntryPoint.h"

class ExampleLayer : public Strype::Layer
{
public:
	ExampleLayer()
		: m_CameraController(1280.0f / 720.0f)
	{
	}

	void OnAttach()
	{
	}

	void OnUpdate(Strype::Timestep ts) override
	{
		// Update
		m_CameraController.OnUpdate(ts);

		Strype::Renderer::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 colour = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Strype::Renderer::DrawQuad({ x, y, 0.0f }, { 0.45f, 0.45f }, colour);
			}
		}
		Strype::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
	}

	void OnEvent(Strype::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}

private:
	Strype::Ref<Strype::Texture> m_Texture;

	Strype::CameraController m_CameraController;
};

class Sandbox : public Strype::Application
{
public:
	Sandbox(const Strype::AppConfig& config)
		: Strype::Application(config)
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}

};

Strype::Application* Strype::CreateApplication()
{
	AppConfig config;
	config.DockspaceEnabled = false;

	return new Sandbox(config);
}