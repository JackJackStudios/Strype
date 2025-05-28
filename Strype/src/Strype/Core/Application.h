#include "stypch.h"

#include "Strype/Core/Base.h"

#include "Strype/Core/Window.h"
#include "Strype/Core/LayerStack.h"
#include "Strype/Events/Event.h"
#include "Strype/Events/ApplicationEvent.h"

#include "Strype/Core/Timestep.h"

#include "API/ImGui/ImGuiLayer.h"

#include <AGI/agi.h>

int main(int argc, char** argv);

namespace Strype {

	struct AppConfig
	{
		std::string WorkingDir;
		std::string AppName;
		std::filesystem::path MasterDir;

		bool UseObjectID = false;
		WindowProps WindowProps;

		bool DockspaceEnabled = false;
		bool ImGuizmoEnabled = false;
		bool ImGuiEnabled = true;
		int StartupFrames = 10;
	};

	struct ApplicationArguments
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			if (index >= Count && index < 0)
				return "";
				
			return Args[index];
		}
	};

	class Application
	{
	public:
		Application(const AppConfig& config);
		virtual ~Application();

		Window& GetWindow() { return *m_Window; }
		ApplicationArguments& GetArgs() { return m_Arguments; }

		void Close();

		static Application& Get() { return *s_Instance; }

		const AppConfig& GetConfig() const { return m_Config; }
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		void OnEvent(Event& e);

		template<typename T, typename... Args>
		void PushLayer(Args&&... args)
		{
			m_LayerStack.PushLayer(new T(std::forward<Args>(args)...));
		}

		template<typename T, typename... Args>
		void PushOverlay(Args&&... args)
		{
			m_LayerStack.PushOverlay(new T(std::forward<Args>(args)...));
		}
	private:
		void Run();
		void SetArgs(ApplicationArguments args) { m_Arguments = args; }
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		AppConfig m_Config;
		ApplicationArguments m_Arguments;

		bool m_Running = true;
		bool m_Minimized = false;

		int m_StartupFrames;
		float m_LastFrameTime = 0.0f;
		LayerStack m_LayerStack;
	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication(ApplicationArguments args);

}