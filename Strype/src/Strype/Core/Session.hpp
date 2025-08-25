#pragma once

#include "Strype/Core/Base.hpp"
#include "Strype/Core/Event.hpp"

#include "Strype/Renderer/Renderer.hpp"

#include <AGI/agi.hpp>
#include <ImGuiLayer.hpp>

namespace Strype {

	class Session
	{
	public:
		virtual ~Session() {}

		virtual void OnAttach() {}
		virtual void OnUpdate(float ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		AGI::Window* GetWindow() const
		{
			return Render->GetContext()->GetBoundWindow();
		}
	public:
		Scope<Renderer> Render;

		AGI::WindowProps WindowProps;
		bool ImGuiEnabled = false;
		bool DockspaceEnabled = false;

		bool Running = false;
	private:
		int m_StackIndex;

		EventQueue m_EventQueue;
		std::unique_ptr<AGI::ImGuiLayer> m_ImGuiLayer;
		int m_StartupFrames;

		friend class Application;
	};

}