#include "stypch.h"
#include "Application.h"

#include "Strype/Renderer/Renderer.h"

//TEMP
#include <GLFW/glfw3.h>

namespace Strype {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		STY_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Window::Create();
		m_Window->SetEventCallback(STY_BIND_EVENT_FN(Application::OnEvent));
		Audio::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		Renderer::Shutdown();
		Audio::Shutdown();
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
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
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			Strype::Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			Strype::Renderer::Clear();

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(timestep);

			m_ImGuiLayer->Begin();
			{
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();
			
			m_Window->OnUpdate();
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

