#include "stypch.hpp"
#include "Application.hpp"

#include "Strype/Core/Input.hpp"
#include "Strype/Renderer/Renderer.hpp"

#include "Strype/Utils/PlatformUtils.hpp"
#include "Strype/Utils/ScopedDockspace.hpp"
#include "Strype/Script/ScriptEngine.hpp"

#include <ImGuizmo.h>

namespace Strype {

	static void OnAGIMessage(std::string_view message, AGI::LogLevel level)
	{
		switch (level)
		{
		case AGI::LogLevel::Trace:   STY_LOG_TRACE("Renderer", message); break;
		case AGI::LogLevel::Info:    STY_LOG_INFO("Renderer", message); break;
		case AGI::LogLevel::Warning: STY_LOG_WARN("Renderer", message); break;
		case AGI::LogLevel::Error:   STY_LOG_ERROR("Renderer", message); break;
		}
	}

	static void AudioCallback(void* pUserData, ma_uint32 level, const char* pMessage)
	{
		switch (level)
		{
		//case MA_LOG_LEVEL_DEBUG:   STY_LOG_TRACE("Audio", std::string_view(pMessage)); break;
		//case MA_LOG_LEVEL_INFO:    STY_LOG_INFO("Audio", std::string_view(pMessage)); break;
		case MA_LOG_LEVEL_WARNING: STY_LOG_WARN("Audio", std::string_view(pMessage)); break;
		case MA_LOG_LEVEL_ERROR:   STY_LOG_ERROR("Audio", std::string_view(pMessage)); break;
		}
	}

	Application::Application(int argc, char** argv)
	{
		STY_VERIFY(!s_Instance, "Application already exists!");
		s_Instance = this;

		Log::Init();

		ma_log logger;
		ma_log_init(NULL, &logger);
		ma_log_register_callback(&logger, (ma_log_callback)AudioCallback);

		ma_engine_config config = ma_engine_config_init();
		config.pLog = &logger;

		ma_result result = ma_engine_init(&config, &m_AudioEngine);
		STY_VERIFY(result == MA_SUCCESS, "Failed to initialize audio");

		STY_LOG_INFO("Audio",  "Using audio device: {}", ma_engine_get_device(&m_AudioEngine)->playback.name);
		STY_LOG_TRACE("Audio", "  Format: {}", ma_get_format_name(ma_engine_get_device(&m_AudioEngine)->playback.format));
		STY_LOG_TRACE("Audio", "  Channels: {}", ma_engine_get_device(&m_AudioEngine)->playback.channels);
		STY_LOG_TRACE("Audio", "  Sample Rate: {}", ma_engine_get_device(&m_AudioEngine)->sampleRate);
	}

	Application::~Application()
	{
		for (auto& thread : m_ActiveThreads)
			thread.join();

		ma_engine_uninit(&m_AudioEngine);
	}

	void Application::OnEvent(Event& e)
	{
		STY_VERIFY(s_CurrentSession, "Cannot call OnEvent() on this thread");

		s_CurrentSession->OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(STY_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<ApplicationQuitEvent>(STY_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(STY_BIND_EVENT_FN(Application::OnWindowResize));
	}

	void Application::InitSession(Session* session)
	{
		AGI::Settings settings;
		settings.PreferedAPI = AGI::BestAPI();
		settings.MessageFunc = OnAGIMessage;
		settings.Blending = true;
		settings.ShareResources = session->ShareResources;

		session->WindowProps.Visible = (session->StartupFrames == 0);

		auto* window = AGI::Window::Create(settings, session->WindowProps);
		session->Render = CreateScope<Renderer>(window);

		session->m_StackIndex = m_ActiveSessions.size() - 1;
		m_ActiveThreads.emplace_back(STY_BIND_EVENT_FN(Application::ThreadFunc), session);
	}

	void Application::Run()
	{
		m_IsRunning = true;

		while (!m_ActiveSessions.empty())
		{
			while (!m_GlobalQueue.empty())
			{
				Event* event = m_GlobalQueue.front();
				for (auto& session : m_ActiveSessions) session->m_EventQueue.push_back(event);

				m_GlobalQueue.pop_front();
			}
		}
	}

	void Application::OnWindowClose(Event& e)
	{
		auto window = s_CurrentSession->GetWindow();
		window->ShouldClose(true);
		window->SetVisable(false);
	}

	void Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetSize().x == 0 || e.GetSize().y == 0)
			return;

		Renderer::GetCurrent()->OnWindowResize(e.GetSize());
	}

	void Application::InstallCallbacks()
	{
		auto window = s_CurrentSession->GetWindow();
		window->SetWindowSizeCallback([](AGI::Window* window, glm::vec2 size)
		{
			Application::Get().DispatchEvent<WindowResizeEvent>(size);
		});

		window->SetWindowPosCallback([](AGI::Window* window, glm::vec2 pos)
		{
			Application::Get().DispatchEvent<WindowMoveEvent>(pos);
		});

		window->SetWindowCloseCallback([](AGI::Window* window)
		{
			Application::Get().DispatchEvent<WindowCloseEvent>();
		});

		window->SetDropCallback([](AGI::Window* window, int path_count, const char* paths[])
		{
			std::vector<std::filesystem::path> filepaths(path_count);
			for (int i = 0; i < path_count; i++)
				filepaths[i] = paths[i];

			Application::Get().DispatchEvent<WindowDropEvent>(std::move(filepaths));
		});

		window->SetScrollCallback([](AGI::Window* window, glm::vec2 offset)
		{
			Application::Get().DispatchEvent<MouseScrolledEvent>(offset);
		});

		window->SetCursorPosCallback([](AGI::Window* window, glm::vec2 pos)
		{
			Application::Get().DispatchEvent<MouseMovedEvent>(pos);
		});
	}

	void Application::ThreadFunc(Session* session)
	{
		s_CurrentSession = session;
		session->Render->Init();

		InstallCallbacks();
		Input::Init();

		if (session->ImGuiEnabled)
		{
			session->m_ImGuiLayer = std::make_unique<AGI::ImGuiLayer>(session->Render->GetContext());

			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.IniFilename = "assets/imgui.ini";

			io.Fonts->AddFontFromFileTTF("assets/Roboto-Regular.ttf", 16);
		}

		auto* window = session->GetWindow();
		session->OnAttach();

		while (!window->ShouldClose())
		{
			float timestep = window->GetDelta();

			session->Render->SetClearColour({ 0.1f, 0.1f, 0.1f });
			session->Render->BeginFrame();

			session->OnUpdate(timestep);

			if (session->m_ImGuiLayer)
			{
				session->m_ImGuiLayer->BeginFrame();
				ImGuizmo::BeginFrame();

				{
					ScopedDockspace dockspace(session->DockspaceEnabled);
					session->OnImGuiRender();
				}

				session->m_ImGuiLayer->EndFrame();
			}

			if (session->StartupFrames >= 0)
			{
				if (session->StartupFrames == 0)
					window->SetVisable(true);
			
				session->StartupFrames--;
			}

			session->Render->EndFrame();
			window->PollEvents();

			Input::Update();
			while (!session->m_EventQueue.empty())
			{
				Event* event = session->m_EventQueue.front();
				session->m_EventQueue.pop_front();

				OnEvent(*event);
			}
		}

		session->m_ImGuiLayer.reset();
		session->Render.reset();

		int index = session->m_StackIndex;
		delete session;
		
		m_ActiveSessions.erase(m_ActiveSessions.begin() + index);
		for (int i = index; i < m_ActiveSessions.size(); ++i)
			m_ActiveSessions[i]->m_StackIndex = i;
	}

};