#include <Strype.h>
#include <Strype/Core/EntryPoint.h>

namespace Strype {

	class ExampleLayer : public Layer
	{
	public:
		ExampleLayer()
			: m_CameraController(1280.0f / 720.0f)
		{
		}

		void OnAttach()
		{
			m_Texture = Texture::Create("assets/textures/Checkerboard.png");
		}

		void OnUpdate(Timestep ts) override
		{
			// Update
			m_CameraController.OnUpdate(ts);

			Renderer::BeginScene(m_CameraController.GetCamera());

			Renderer::DrawQuad({ 0.0f, 0.0f, 0.0f }, { 20.0f, 20.0f }, m_Texture);

			for (float y = -5.0f; y < 5.0f; y += 0.5f)
			{
				for (float x = -5.0f; x < 5.0f; x += 0.5f)
				{
					glm::vec4 colour = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
					Renderer::DrawQuad({ x, y, 0.0f }, { 0.45f, 0.45f }, colour);
				}
			}

			Renderer::EndScene();
		}

		virtual void OnImGuiRender() override
		{
		}

		void OnEvent(Event& e) override
		{
			m_CameraController.OnEvent(e);
		}

	private:
		Ref<Texture> m_Texture;

		CameraController m_CameraController;
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