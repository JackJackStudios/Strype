#include "stypch.hpp"

#include "Strype/Core/Base.hpp"

#include "Strype/Core/Event.hpp"
#include "Strype/Core/Session.hpp"
#include "Strype/Renderer/Renderer.hpp"

#include <AGI/agi.hpp>
#include <ImGuiLayer.hpp>

int main(int argc, char** argv);

namespace Strype {

	struct AppConfig
	{
		std::filesystem::path MasterDir;
		std::filesystem::path WorkingDir;
		
		int StartupFrames = 10;
	};

	class Application
	{
	public:
		Application(const AppConfig& config);
		virtual ~Application();

		static Application& Get() { return *s_Instance; }
		const AppConfig& GetConfig() const { return m_Config; }
		AGI::Window* GetWindow() const { return s_CurrentSession->GetWindow(); }

		void Quit();

		template<typename T, typename... Args>
		Application& NewSession(Args&&... args)
		{
			static_assert(std::is_base_of<Session, T>::value, "T must inherit from Session");

			InitSession(m_ActiveSessions.emplace_back(new T(std::forward<Args>(args)...)));
			return *this;
		}

		template <typename TEvent, typename... Args>
		Application& DispatchEvent(Args&&... args)
		{
			static_assert(std::is_base_of<Event, TEvent>::value, "T must inherit from Event");

			Event* e = new TEvent(std::forward<Args>(args)...);
			e->Dispatched = true;

			if (e->IsGlobal())
			{
				m_GlobalQueue.push_back(e);
			}
			else
			{
				s_CurrentSession->m_EventQueue.push_back(e);
			}

			return *this;
		}

	private:
		void OnEvent(Event& e);

		void InitSession(Session* session);
		void ThreadFunc(Session* layer);

		void OnWindowClose(WindowCloseEvent& e);
		void OnWindowResize(WindowResizeEvent& e);
		void InstallCallbacks();
	protected:
		void Run();
		friend int ::main(int argc, char** argv);
	private:
		AppConfig m_Config;
		bool m_IsRunning = false;

		EventQueue m_GlobalQueue;

		std::vector<Session*> m_ActiveSessions;
		std::vector<std::thread> m_ActiveThreads;
	private:
		inline static Application* s_Instance = nullptr;

		// NOTE: This should be fine since there will only 
		//       ever be one Application class running at once anyway.
		inline static thread_local Session* s_CurrentSession = nullptr;
	};

}