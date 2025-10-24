#include "stypch.hpp"

#include "Strype/Core/Base.hpp"

#include "Strype/Core/Event.hpp"
#include "Strype/Core/Session.hpp"
#include "Strype/Renderer/Renderer.hpp"

#include <AGI/agi.hpp>
#include <miniaudio.h>
#include <agi_imgui/ImGuiLayer.hpp>

int main(int argc, char** argv);

namespace Strype {

	class Application
	{
	public:
		Application(int argc, char** argv);
		~Application();

		static Application& Get() { return *s_Instance; }
		static bool IsRunning() { return s_Instance != nullptr && s_Instance->m_IsRunning; }

		AGI::Window* GetWindow() const { return s_CurrentSession->GetWindow(); }
		ma_engine* GetAudioEngine() { return &m_AudioEngine; }

		template<typename TSession, typename... Args>
		Application& NewSession(Args&&... args)
		{
			static_assert(std::is_base_of<Session, TSession>::value, "T must inherit from Session");

			InitSession(m_ActiveSessions.emplace_back(new TSession(std::forward<Args>(args)...)));
			return *this;
		}

		template<typename TEvent, typename... Args>
		Application& DispatchEvent(Args&&... args)
		{
			static_assert(std::is_base_of<Event, TEvent>::value, "T must inherit from Event");
			Event* e = new TEvent(std::forward<Args>(args)...);

			if (e->IsGlobal() && m_ActiveSessions.size() > 1)
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
		void ThreadFunc(Session* session);

		void OnWindowClose(Event& e);
		void OnWindowResize(WindowResizeEvent& e);
		void InstallCallbacks();
	protected:
		void Run();
		friend int ::main(int argc, char** argv);
	private:
		bool m_IsRunning = false;
		ma_engine m_AudioEngine = {0};

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