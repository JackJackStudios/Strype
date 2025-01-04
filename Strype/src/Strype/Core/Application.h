#include "stypch.h"

#include "Base.h"

#include "Window.h"
#include "Strype/Core/LayerStack.h"
#include "Strype/Events/Event.h"
#include "Strype/Events/ApplicationEvent.h"

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

		Ref<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	private:
		static Application* s_Instance;
	};

	Application* CreateApplication();

}