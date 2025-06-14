#include "stypch.hpp"

#include "Strype/Core/Base.hpp"

#include "Strype/Core/Window.hpp"
#include "Strype/Core/LayerStack.hpp"
#include "Strype/Events/Event.hpp"
#include "Strype/Events/ApplicationEvent.hpp"

#include "Strype/Renderer/Renderer.hpp"

#include "Strype/Core/Timestep.hpp"

#include "API/ImGui/ImGuiLayer.hpp"

#include <AGI/agi.hpp>

int main(int argc, char** argv);

namespace Strype {

	struct AppConfig
	{
		std::string AppName;
		std::filesystem::path MasterDir;
		std::filesystem::path WorkingDir;

		WindowProps WindowProps;

		bool DockspaceEnabled = false;
		bool ImGuizmoEnabled = false;
		bool ImGuiEnabled = true;
		int StartupFrames = 10;
	};

	class Application
	{
	public:
		Application(const AppConfig& config);
		virtual ~Application();

		Window* GetWindow() { return m_Window.get(); }

		void Close();

		static Application& Get() { return *s_Instance; }

		const AppConfig& GetConfig() const { return m_Config; }
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		void OnEvent(Event& e);

		template<typename T, typename... Args>
		Application& PushPipeline(Args&&... args)
		{
			Renderer::PushPipeline<T>(std::forward<Args>(args)...);
			return *this;
		}

		template<typename T, typename... Args>
		Application& PushLayer(Args&&... args)
		{
			m_LayerStack.PushLayer(new T(std::forward<Args>(args)...));
			return *this;
		}

		template<typename T, typename... Args>
		Application& PushOverlay(Args&&... args)
		{
			m_LayerStack.PushOverlay(new T(std::forward<Args>(args)...));
			return *this;
		}
	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		AppConfig m_Config;

		bool m_Running = true;
		bool m_Minimized = false;

		int m_StartupFrames;
		float m_LastFrameTime = 0.0f;
		LayerStack m_LayerStack;
	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication(int argc, char** argv);

}