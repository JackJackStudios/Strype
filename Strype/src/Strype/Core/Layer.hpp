#pragma once

#include "Strype/Core/Base.hpp"
#include "Strype/Core/Event.hpp"

#include "Strype/Renderer/Renderer.hpp"

#include <AGI/agi.hpp>
#include <AGI/ext/ImGuiLayer.hpp>

namespace Strype {

	class Layer
	{
	public:
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnUpdate(float ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}
	public:
		Scope<Renderer> Render;

		AGI::WindowProps WindowProps;
		bool ImGuiEnabled = false;
	private:
		int m_StackIndex;

		std::unique_ptr<AGI::ImGuiLayer> m_ImGuiLayer;
		int m_StartupFrames;

		friend class Application;
	};

}