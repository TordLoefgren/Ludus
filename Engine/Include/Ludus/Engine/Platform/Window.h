#pragma once

#include <GLFW/glfw3.h>

#include <Ludus/Engine/Core/Utilities.h>
#include <Ludus/Engine/Events/Event.h>
#include <Ludus/Engine/Events/EventBus.h>
#include <Ludus/Engine/Events/EventHandler.h>
#include <Ludus/Engine/Graphics/FramebufferSize.h>
#include <Ludus/Engine/Platform/WindowOptions.h>
#include <Ludus/Engine/Platform/WindowUserData.h>

namespace Ludus::Engine::Platform
{
	class Window
	{
	private:
		GLFWwindow* m_Handle;
		WindowOptions m_WindowOptions;
		Ludus::Engine::Events::EventBus& m_EventBus;
		WindowUserData m_UserData;

		void Init();
		void SetEventCallbacks();

	public:
		Window(WindowOptions options, Ludus::Engine::Events::EventBus& eventBus);
		~Window();

		void PollEvents();
		void SwapBuffers();
		void SetWindowShouldClose();
		Ludus::Engine::Graphics::FramebufferSize GetFramebufferSize() const;
		bool WindowShouldClose();
		void SetVSync(bool enable);

		const WindowOptions GetOptions() { return m_WindowOptions; }
		GLFWwindow* GetHandle() const { return m_Handle; }
	};
}
