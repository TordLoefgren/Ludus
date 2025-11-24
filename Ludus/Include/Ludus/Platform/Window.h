#pragma once

#include <GLFW/glfw3.h>

#include <Ludus/Engine/Utilities.h>
#include <Ludus/Events/Event.h>
#include <Ludus/Events/EventBus.h>
#include <Ludus/Events/EventHandler.h>
#include <Ludus/Platform/WindowOptions.h>
#include <Ludus/Platform/WindowUserData.h>

namespace Ludus::Platform
{
	class Window
	{
	private:
		GLFWwindow* m_Handle;
		WindowOptions m_WindowOptions;
		Ludus::Events::EventBus& m_EventBus;
		WindowUserData m_UserData;

		void Init();
		void SetEventCallbacks();

	public:
		Window(WindowOptions options, Ludus::Events::EventBus& eventBus);
		~Window();

		void PollEvents();
		void SwapBuffers();
		void SetWindowShouldClose();
		bool WindowShouldClose();
		void SetVSync(bool enable);

		const WindowOptions GetOptions() { return m_WindowOptions; }
	};
}
