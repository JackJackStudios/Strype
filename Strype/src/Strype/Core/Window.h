#pragma once

#include "stypch.h"

#include "Strype/Core/Base.h"
#include "Strype/Events/Event.h"

#include <GLFW/glfw3.h>

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

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled);
		bool IsVSync() const;
		void SetVisable(bool enabled);

		GLFWwindow* GetNativeWindow() const { return m_Window; }

		static inline Scope<Window> Create(const WindowProps& props = WindowProps()) { return CreateScope<Window>(props); }
	private:
		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};

}