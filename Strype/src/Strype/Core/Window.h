#pragma once

#include "stypch.h"

#include "Strype/Core/Base.h"
#include "Strype/Events/Event.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace Strype {

	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		bool VSync;

		WindowProps(const std::string& title = "Strype Engine",
			unsigned int width = 1280,
			unsigned int height = 720,
			bool vsync = true)
			: Title(title), Width(width), Height(height), VSync(vsync)
		{
		}
	};

	// Interface representing a desktop system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window(const WindowProps& props);
		~Window();

		void OnUpdate();

		inline unsigned int GetWidth() const { return m_Data.Width; }
		inline unsigned int GetHeight() const { return m_Data.Height; }
		glm::vec2 GetPosition() const;

		// Window attributes
		void SetVSync(bool enabled);
		bool IsVSync() const;
		void SetVisable(bool enabled);
		void SetTitle(const std::string& title);

		GLFWwindow* GetNativeWindow() const { return m_Window; }

		static inline Scope<Window> Create(const WindowProps& props = WindowProps()) { return CreateScope<Window>(props); }
	private:
		GLFWwindow* m_Window;
		WindowProps m_Data;
	};

}