#include "stypch.hpp"

#include "Strype/Core/Base.hpp"

#include "Strype/Core/Event.hpp"
#include "Strype/Core/Layer.hpp"
#include "Strype/Renderer/Renderer.hpp"

#include <AGI/agi.hpp>
#include <AGI/ext/ImGuiLayer.hpp>

int main(int argc, char** argv);

namespace Strype {

	struct AppConfig
	{
		std::string AppName;
		std::filesystem::path MasterDir;
		std::filesystem::path WorkingDir;

		AGI::WindowProps WindowProps;

		bool DockspaceEnabled = false;
		bool ImGuiEnabled = true;
		int StartupFrames = 10;
	};

	class Application
	{
	public:
		Application(const AppConfig& config);
		virtual ~Application();

		static Application& Get() { return *s_Instance; }

		void Close();
		
		const AppConfig& GetConfig() const { return m_Config; }
		std::unique_ptr<AGI::Window>& GetWindow() { return m_Window; }

		void OnEvent(Event& e);

		template<typename T, typename... Args>
		Application& PushLayer(Args&&... args)
		{
			m_LayerStack.emplace_back(new T(std::forward<Args>(args)...));
			return *this;
		}
	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		void InstallCallbacks();
	private:
		AppConfig m_Config;

		std::unique_ptr<AGI::Window> m_Window;
		std::unique_ptr<AGI::ImGuiLayer> m_ImGuiLayer;

		bool m_Running = true;
		bool m_Minimized = false;

		int m_StartupFrames;
		float m_LastFrameTime = 0.0f;
		std::vector<Layer*> m_LayerStack;
	private:
		static Application* s_Instance;

		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication(int argc, char** argv);

}