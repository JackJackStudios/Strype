#include "stypch.hpp"
#include "Application.hpp"

#include "Strype/Renderer/Renderer.hpp"
#include "Strype/Audio/Audio.hpp"
#include "Strype/Core/Input.hpp"

#include "Strype/Script/ScriptEngine.hpp"

#include "Strype/Utils/PlatformUtils.hpp"

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

		m_StartupFrames = m_Config.StartupFrames;
		m_Config.WindowProps.Title = config.WindowProps.Title != AGI::WindowProps().Title ? config.WindowProps.Title : config.AppName;
		m_Config.WindowProps.Visible = false;

		Audio::Init();
		ScriptEngine::Initialize();
	}

	Application::~Application()
	{
		for (auto& thread : m_ActiveThreads)
			thread.join();

		ScriptEngine::Shutdown();
		Audio::Shutdown();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(STY_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(STY_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.begin(); it != m_LayerStack.end(); ++it)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
	}

	void Application::Close()
	{
		m_Window->ShouldClose(true);
		m_Window->SetVisable(false);
	}

	void Application::ThreadFunc(Layer* layer)
	{
		layer->Renderer->Init();
		m_Window = layer->Renderer->GetContext()->GetBoundWindow();

		InstallCallbacks(m_Window);
		layer->OnAttach();

		while (!m_Window->ShouldClose())
		{
			float time = m_Window->GetTime();
			float timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			layer->Renderer->SetClearColour({ 0.1f, 0.1f, 0.1f, 1 });
			layer->Renderer->Clear();

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(timestep);

			if (m_Config.ImGuiEnabled)
			{
				layer->ImGuiLayer->BeginFrame(false);
				ImGuizmo::BeginFrame();
			
				if (m_Config.DockspaceEnabled)
				{
					ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar;
					window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
					window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			
					ImGuiViewport* viewport = ImGui::GetMainViewport();
					ImGui::SetNextWindowPos(viewport->Pos);
					ImGui::SetNextWindowSize(viewport->Size);
					ImGui::SetNextWindowViewport(viewport->ID);
			
					ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			
					ImGui::Begin("DockSpace", 0, window_flags);
			
					ImGui::PopStyleVar(3);
			
					ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
					ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));
				}
			
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
			
				if (m_Config.DockspaceEnabled)
					ImGui::End();
			
				layer->ImGuiLayer->EndFrame();
			}

			m_Window->OnUpdate();

			if (--m_StartupFrames == 0)
				m_Window->SetVisable(true);
		}

		m_Window->SetVisable(false);

		if (m_Config.ImGuiEnabled)
			layer->ImGuiLayer.reset();

		layer->Renderer->Shutdown();
		delete layer;
	}

	void Application::Run()
	{
		for (int i=m_LayerStack.size()-1; i >= 0; i--)
		{
			Layer* layer = m_LayerStack[i];

			AGI::Settings settings;
			settings.PreferedAPI = AGI::BestAPI();
			settings.MessageFunc = OnAGIMessage;
			settings.Blending = true;

			settings.Window = m_Config.WindowProps;
			auto* window = AGI::Window::Create(settings);

			layer->Renderer = CreateScope<Renderer>(window);
			layer->Renderer->Init();

			InstallCallbacks(window);

			if (m_Config.ImGuiEnabled)
			{
				layer->ImGuiLayer = AGI::ImGuiLayer::Create(window);

				ImGuiIO& io = ImGui::GetIO();
				io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
				io.IniFilename = "assets/imgui.ini";

				if (m_Config.DockspaceEnabled)
					io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			}

			if (i > 0)
			{
				// Spawn thread
				m_ActiveThreads.emplace_back(STY_BIND_EVENT_FN(Application::ThreadFunc), layer);
			}
			else
			{
				ThreadFunc(layer);
			}
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		Close();
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetSize().x == 0 || e.GetSize().y == 0)
			return false;

		Renderer::GetCurrent()->OnWindowResize(e.GetSize());
		return false;
	}

	void Application::InstallCallbacks(AGI::Window* window)
	{
		window->SetWindowSizeCallback([](AGI::Window* _window, glm::vec2 size)
		{
			WindowResizeEvent event(size);
			Application::Get().OnEvent(event);
		});

		window->SetWindowPosCallback([](AGI::Window* _window, glm::vec2 pos)
		{
			WindowMoveEvent event(pos);
			Application::Get().OnEvent(event);
		});

		window->SetWindowCloseCallback([](AGI::Window* _window)
		{
			WindowCloseEvent event;
			Application::Get().OnEvent(event);
		});

		window->SetDropCallback([](AGI::Window* _window, int path_count, const char* paths[])
		{
			std::vector<std::filesystem::path> filepaths(path_count);
			for (int i = 0; i < path_count; i++)
				filepaths[i] = paths[i];

			WindowDropEvent event(std::move(filepaths));
			Application::Get().OnEvent(event);
		});

		window->SetScrollCallback([](AGI::Window* _window, glm::vec2 offset)
		{
			MouseScrolledEvent event(offset);
			Application::Get().OnEvent(event);
		});

		window->SetCursorPosCallback([](AGI::Window* _window, glm::vec2 pos)
		{
			MouseMovedEvent event(pos);
			Application::Get().OnEvent(event);
		});
	}

}

