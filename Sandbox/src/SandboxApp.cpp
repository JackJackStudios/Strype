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
		{
		}
		Strype::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(Strype::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}

private:
	Strype::Ref<Strype::Shader> m_Shader;
	Strype::Ref<Strype::VertexArray> m_SquareVA;
	Strype::Ref<Strype::Texture> m_Texture;

	Strype::CameraController m_CameraController;
	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
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