#include "stypch.hpp"
#include "Application.hpp"

#include "Strype/Renderer/Renderer.hpp"
#include "Strype/Audio/Audio.hpp"
#include "Strype/Core/Input.hpp"

#include "Strype/Utils/PlatformUtils.hpp"
#include "Strype/Utils/ScopedDockspace.hpp"

#include "Strype/Script/ScriptEngine.hpp"

#include <ImGuizmo.h>

namespace Strype {

	static void OnAGIMessage(std::string_view message, AGI::LogLevel level)
	{
		switch (level)
		{
		case AGI::LogLevel::Trace:   STY_CORE_TRACE("{}", std::string(message)); break;
		case AGI::LogLevel::Info:    STY_CORE_INFO("{}", std::string(message)); break;
		case AGI::LogLevel::Warning: STY_CORE_WARN("{}", std::string(message)); break;
		case AGI::LogLevel::Error:   STY_CORE_ERROR("{}", std::string(message)); break;
		}
	}

	Application::Application(const AppConfig& config)
		: m_Config(config)
	{
		Log::Init();

		STY_CORE_VERIFY(!s_Instance, "Application already exists!");
		s_Instance = this;

		if (!m_Config.WorkingDir.empty())
			std::filesystem::current_path(m_Config.WorkingDir);

		Audio::Init();
	}

	Application::~Application()
	{
		for (auto& thread : m_ActiveThreads)
			thread.join();

		Audio::Shutdown();
	}

	void Application::OnEvent(Event& e)
	{
		STY_CORE_VERIFY(s_CurrentSession, "Cannot call OnEvent() on this thread");
		STY_CORE_VERIFY(e.Dispatched, "Cannot call OnEvent() directly, use DispatchEvent instead");

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

		session->WindowProps.Visible = false;

		auto* window = AGI::Window::Create(settings, session->WindowProps);
		session->Render = CreateScope<Renderer>(window);

		session->m_StartupFrames = m_Config.StartupFrames;
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
				m_GlobalQueue.pop_front();
				
				for (auto& session : m_ActiveSessions) session->m_EventQueue.push_back(event);
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
		STY_CORE_VERIFY(session->Running == false, "Session already active");
		session->Running = true;
		
		session->Render->Init();

		InstallCallbacks();
		Input::Init();

		if (session->ImGuiEnabled)
		{
			session->m_ImGuiLayer = AGI::ImGuiLayer::Create(session->Render->GetContext());

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
			session->Render->Clear();

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

			if (--session->m_StartupFrames == 0)
				window->SetVisable(true);

			window->OnUpdate();
			Input::Update();

			while (!session->m_EventQueue.empty())
			{
				Event* event = session->m_EventQueue.front();
				session->m_EventQueue.pop_front();

				OnEvent(*event);
			}
		}

		session->m_ImGuiLayer.reset();
		session->Render->Shutdown();

		int index = session->m_StackIndex;
		delete session;

		m_ActiveSessions.erase(m_ActiveSessions.begin() + index);
		for (int i = index; i < m_ActiveSessions.size(); ++i)
			m_ActiveSessions[i]->m_StackIndex = i;
	}

};