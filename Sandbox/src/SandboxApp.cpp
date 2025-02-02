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
			m_Room = CreateRef<Room>();

			m_Texture = Texture::Create("assets/textures/Checkerboard.png");

			Object obj = m_Room->CreateObject();
			obj.AddComponent<Transform>();
			obj.AddComponent<SpriteRenderer>(m_Texture);
		}

		void OnUpdate(Timestep ts) override
		{
			m_Room->OnUpdate(ts);
		}

		virtual void OnImGuiRender() override
		{
		}

		void OnEvent(Event& e) override
		{
			m_Room->OnEvent(e);
		}

	private:
		Ref<Texture> m_Texture;

		Ref<Room> m_Room;
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