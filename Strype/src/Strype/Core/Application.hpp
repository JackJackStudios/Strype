#include "stypch.hpp"

#include "Strype/Core/Base.hpp"

#include "Strype/Core/Event.hpp"
#include "Strype/Core/Session.hpp"
#include "Strype/Renderer/Renderer.hpp"

#include <AGI/agi.hpp>
#include <AGI/ext/ImGuiLayer.hpp>

int main(int argc, char** argv);

namespace Strype {

	struct AppConfig
	{
		std::filesystem::path MasterDir;
		std::filesystem::path WorkingDir;

		bool DockspaceEnabled = false;
		int StartupFrames = 10;
	};

	class Application
	{
	public:
		Application(const AppConfig& config);
		virtual ~Application();

		static Application& Get() { return *s_Instance; }
		const AppConfig& GetConfig() const { return m_Config; }

		AGI::Window* GetWindow() { return m_Window; }

		void OnEvent(Event& e);
		void Run();

		template<typename T, typename... Args>
		Application& NewSession(Args&&... args)
		{
			InitSession(m_ActiveSessions.emplace_back(new T(std::forward<Args>(args)...)));
			if (m_IsRunning)
			{
				m_ActiveThreads.emplace_back(STY_BIND_EVENT_FN(Application::ThreadFunc), m_ActiveSessions[m_ActiveSessions.size() - 1]);
			}
			return *this;
		}
	private:
		void InitSession(Session* layer);
		void ThreadFunc(Session* layer);

		void OnApplicationQuit(ApplicationQuitEvent& e);
		
		void OnWindowClose(WindowCloseEvent& e);
		void OnWindowResize(WindowResizeEvent& e);
		void InstallCallbacks();
	private:
		AppConfig m_Config;
		bool m_IsRunning = false;

		std::vector<Session*> m_ActiveSessions;
		std::vector<std::thread> m_ActiveThreads;
	private:
		inline static Application* s_Instance = nullptr;
		inline static thread_local AGI::Window* m_Window;
	};

	Application* CreateApplication(int argc, char** argv);

}