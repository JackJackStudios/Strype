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
		const AppConfig& GetConfig() const { return m_Config; }

		AGI::Window*& GetWindow() { return m_Window; }

		void OnEvent(Event& e);
		void Close();

		template<typename T, typename... Args>
		Application& PushLayer(Args&&... args)
		{
			m_LayerStack.emplace_back(new T(std::forward<Args>(args)...));
			return *this;
		}
	private:
		void ThreadFunc(Layer* context);

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		void InstallCallbacks(AGI::Window* window);
	private:
		AppConfig m_Config;

		int m_StartupFrames;
		float m_LastFrameTime = 0.0f;

		std::vector<Layer*> m_LayerStack;
		std::vector<std::thread> m_ActiveThreads;
	private:
		inline static Application* s_Instance = nullptr;
		inline static thread_local AGI::Window* m_Window;

		void Run();
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication(int argc, char** argv);

}