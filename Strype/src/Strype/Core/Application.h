#include "stypch.h"

#include "Strype/Core/Base.h"

#include "Strype/Core/Window.h"
#include "Strype/Core/LayerStack.h"
#include "Strype/Events/Event.h"
#include "Strype/Events/ApplicationEvent.h"

#include "Strype/Core/Timestep.h"

#include "Strype/ImGui/ImGuiLayer.h"

namespace Strype {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		Window& GetWindow() { return *m_Window; }

		void Close();

		static Application& Get() { return *s_Instance; }

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;

		bool m_Running = true;
		bool m_Minimized = false;

		float m_LastFrameTime = 0.0f;
		LayerStack m_LayerStack;
	private:
		static Application* s_Instance;
	};

	Application* CreateApplication();

}