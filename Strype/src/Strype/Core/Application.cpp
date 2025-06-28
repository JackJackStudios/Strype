#include "stypch.hpp"
#include "Application.hpp"

#include "Strype/Renderer/Renderer.hpp"
#include "Strype/Audio/Audio.hpp"
#include "Strype/Core/Input.hpp"

#include "Strype/Script/ScriptEngine.hpp"

#include "Strype/Utils/PlatformUtils.hpp"

namespace Strype {

	Application* Application::s_Instance = nullptr;

	Application::Application(const AppConfig& config)
		: m_Config(config)
	{
		Log::Init();

		STY_CORE_VERIFY(!s_Instance, "Application already exists!");
		s_Instance = this;

		if (!m_Config.WorkingDir.empty())
			std::filesystem::current_path(m_Config.WorkingDir);

		m_StartupFrames = m_Config.StartupFrames;
		m_Config.WindowProps.Title = config.WindowProps.Title != AGI::WindowProps().Title ? config.WindowProps.Title : config.AppName;
		m_Config.WindowProps.Visible = false;

		Renderer::Init();

		Audio::Init();
		Input::Init();

		ScriptEngine::Initialize();

		InstallCallbacks();

		if (m_Config.ImGuiEnabled)
			m_ImGuiLayer = AGI::ImGuiLayer::Create(m_Window->GetGlfwWindow());
	}

	Application::~Application()
	{
		m_Window->SetVisable(false);

		if (m_Config.ImGuiEnabled)
			m_ImGuiLayer.reset();

		m_LayerStack.DestroyLayers();

		ScriptEngine::Shutdown();

		Renderer::Shutdown();
		Audio::Shutdown();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(STY_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(STY_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
	}

	void Application::Close()
	{
		m_Running = false;
		m_Window->SetVisable(false);
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float time = m_Window->GetTime();
			float timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			Renderer::SetClearColour({ 0.1f, 0.1f, 0.1f, 1 });
			Renderer::Clear();

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(timestep);

			if (m_Config.ImGuiEnabled)
			{
				m_ImGuiLayer->BeginFrame(m_Config.DockspaceEnabled);

				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
					
				m_ImGuiLayer->EndFrame();
			}
			
			m_Window->OnUpdate();
			Input::Update();

			if (--m_StartupFrames == 0)
				m_Window->SetVisable(true);
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		Close();
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

	void Application::InstallCallbacks()
	{
		m_Window->SetWindowSizeCallback([](AGI::Window* window, glm::vec2 size)
		{
			WindowResizeEvent event(size.x, size.y);
			Application::Get().OnEvent(event);
		});

		m_Window->SetWindowPosCallback([](AGI::Window* window, glm::vec2 pos)
		{
			WindowMoveEvent event(pos.x, pos.y);
			Application::Get().OnEvent(event);
		});

		m_Window->SetWindowCloseCallback([](AGI::Window* window)
		{
			WindowCloseEvent event;
			Application::Get().OnEvent(event);
		});

		m_Window->SetDropCallback([](AGI::Window* window, int path_count, const char* paths[])
		{
			std::vector<std::filesystem::path> filepaths(path_count);
			for (int i = 0; i < path_count; i++)
				filepaths[i] = paths[i];

			WindowDropEvent event(std::move(filepaths));
			Application::Get().OnEvent(event);
		});

		m_Window->SetScrollCallback([](AGI::Window* window, glm::vec2 offset)
		{
			MouseScrolledEvent event((float)offset.x, (float)offset.y);
			Application::Get().OnEvent(event);
		});

		m_Window->SetCursorPosCallback([](AGI::Window* window, glm::vec2 pos)
		{
			MouseMovedEvent event((float)pos.x, (float)pos.y);
			Application::Get().OnEvent(event);
		});
	}

}

