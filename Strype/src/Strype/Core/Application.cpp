#include "stypch.h"
#include "Application.h"

#include "Strype/Renderer/Renderer.h"
#include "Strype/Core/Audio.h"
#include "Strype/Core/Input.h"

#include "Strype/Script/ScriptEngine.h"

#include "Strype/Utils/PlatformUtils.h"

namespace Strype {

	Application* Application::s_Instance = nullptr;

	Application::Application(const AppConfig& config)
		: m_Config(config)
	{
		STY_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		if (!m_Config.WorkingDir.empty())
			std::filesystem::current_path(m_Config.WorkingDir);
		m_StartupFrames = m_Config.StartupFrames;

		m_Window = Window::Create();
		m_Window->SetEventCallback(STY_BIND_EVENT_FN(Application::OnEvent));
		
		Input::SetEventCallback(STY_BIND_EVENT_FN(Application::OnEvent));
		Audio::Init();

		ScriptEngine::Initialize();

		m_ImGuiLayer = new ImGuiLayer(m_Config.DockspaceEnabled);
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		m_Window->SetVisable(false);

		ScriptEngine::Shutdown();

		Renderer::Shutdown();
		Audio::Shutdown();
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
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
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float time = Time::GetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			Renderer::SetClearColour({ 0.1f, 0.1f, 0.1f, 1 });
			Renderer::Clear();

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(timestep);

			m_ImGuiLayer->Begin();
			{
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();
			
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

}

