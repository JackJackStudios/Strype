#include "stypch.h"

#include "Strype/Core/Base.h"

#include "Strype/Core/Window.h"
#include "Strype/Core/LayerStack.h"
#include "Strype/Events/Event.h"
#include "Strype/Events/ApplicationEvent.h"

#include "Strype/Core/Timestep.h"
#include "Strype/Core/Project.h"

#include "API/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Strype {

	struct AppConfig
	{
		std::string WorkingDir;
		bool DockspaceEnabled;
	};

	class Application
	{
	public:
		Application(const AppConfig& config);
		virtual ~Application();

		Window& GetWindow() { return *m_Window; }

		void Close();

		static Application& Get() { return *s_Instance; }

		const AppConfig& GetConfig() const { return m_Config; }

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		AppConfig m_Config;

		bool m_Running = true;
		bool m_Minimized = false;

		float m_LastFrameTime = 0.0f;
		LayerStack m_LayerStack;
	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication();

}