#include "Strype.h"
#include "Strype/Core/EntryPoint.h"

class ExampleLayer : public Strype::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_CameraController(1280.0f / 720.0f)
	{
	}

	void OnAttach()
	{
		m_SquareVA = Strype::VertexArray::Create();

		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		Strype::Ref<Strype::VertexBuffer> squareVB = Strype::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->SetLayout({
			{ Strype::ShaderDataType::Float3, "a_Position" },
			{ Strype::ShaderDataType::Float2, "a_TexCoord" }	
		});
		m_SquareVA->AddVertexBuffer(squareVB); 

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Strype::Ref<Strype::IndexBuffer> squareIB = Strype::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		m_SquareVA->SetIndexBuffer(squareIB);

		m_Shader = Strype::Shader::Create("assets/shaders/BaseShader.glsl");
		m_Texture = Strype::Texture::Create("assets/textures/Checkerboard.png");

		m_Shader->Bind();
		m_Shader->SetInt("u_Texture", 0);
	}

	void OnUpdate(Strype::Timestep ts) override
	{
		// Update
		m_CameraController.OnUpdate(ts);

		Strype::Renderer::BeginScene(m_CameraController.GetCamera());
		{
			m_Shader->Bind();
			m_Shader->SetFloat3("u_Color", m_SquareColor);

			m_Texture->Bind();
			Strype::Renderer::Submit(m_Shader, m_SquareVA);
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
	config.DockspaceEnabled = true;

	return new Sandbox(config);
}