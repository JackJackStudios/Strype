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
		for (auto it = m_LayerStack.begin(); it != m_LayerStack.end(); ++it)
		{
			if (e.Handled) break;
			(*it)->OnEvent(e);
		}

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(STY_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(STY_BIND_EVENT_FN(Application::OnWindowResize));
	}

	void Application::Close()
	{
		m_Window->ShouldClose(true);
		m_Window->SetVisable(false);
	}

	void Application::ThreadFunc(int index)
	{
		Layer* layer = m_LayerStack[index];
		layer->Render->Init();

		m_Window = layer->Render->GetWindow();
		InstallCallbacks();

		Input::Init();

		if (layer->ImGuiEnabled)
		{
			layer->m_ImGuiLayer = AGI::ImGuiLayer::Create(m_Window);

			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.IniFilename = "assets/imgui.ini";

			if (m_Config.DockspaceEnabled)
				io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		}

		layer->OnAttach();

		while (!m_Window->ShouldClose())
		{
			float timestep = m_Window->GetDelta();

			layer->Render->SetClearColour({ 0.1f, 0.1f, 0.1f, 1 });
			layer->Render->Clear();
			
			layer->OnUpdate(timestep);

			if (layer->ImGuiEnabled)
			{
				layer->m_ImGuiLayer->BeginFrame();
				ImGuizmo::BeginFrame();

				{
					ScopedDockspace dockspace(m_Config.DockspaceEnabled);
					layer->OnImGuiRender();
				}

				layer->m_ImGuiLayer->EndFrame();
			}

			m_Window->OnUpdate();
			Input::Update();

			if (--layer->m_StartupFrames == 0)
				m_Window->SetVisable(true);
		}

		Close();

		layer->m_ImGuiLayer.reset();
		layer->Render->Shutdown();

		delete layer;
		m_LayerStack.erase(m_LayerStack.begin() + index);
	}

	void Application::InitLayer(Layer* layer)
	{
		AGI::Settings settings;
		settings.PreferedAPI = AGI::BestAPI();
		settings.MessageFunc = OnAGIMessage;
		settings.Blending = true;

		layer->WindowProps.Visible = false;

		auto* window = AGI::Window::Create(settings, layer->WindowProps);
		layer->Render = CreateScope<Renderer>(window);

		layer->m_StartupFrames = m_Config.StartupFrames;
	}

	void Application::Run()
	{
		m_IsRunning = true;
		for (size_t i = m_LayerStack.size(); i-- > 0; )
		{
			if (i == 0)
			{
				ThreadFunc(i);
			}
			else
			{
				m_ActiveThreads.emplace_back(STY_BIND_EVENT_FN(Application::ThreadFunc), i);
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

	void Application::InstallCallbacks()
	{
		m_Window->SetWindowSizeCallback([](AGI::Window* window, glm::vec2 size)
		{
			WindowResizeEvent event(size);
			Application::Get().OnEvent(event);
		});

		m_Window->SetWindowPosCallback([](AGI::Window* window, glm::vec2 pos)
		{
			WindowMoveEvent event(pos);
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
			MouseScrolledEvent event(offset);
			Application::Get().OnEvent(event);
		});

		m_Window->SetCursorPosCallback([](AGI::Window* window, glm::vec2 pos)
		{
			MouseMovedEvent event(pos);
			Application::Get().OnEvent(event);
		});
	}

}

