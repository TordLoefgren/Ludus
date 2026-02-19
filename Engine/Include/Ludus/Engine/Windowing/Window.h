#pragma once

#include <GLFW/glfw3.h>

#include <Ludus/Engine/Events/Event.h>
#include <Ludus/Engine/Events/EventBus.h>
#include <Ludus/Engine/Events/EventHandler.h>
#include <Ludus/Engine/Math/Size.h>
#include <Ludus/Engine/Windowing/WindowOptions.h>
#include <Ludus/Engine/Windowing/WindowUserData.h>

namespace Ludus::Engine::Windowing
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
		bool WindowShouldClose();
		void SetVSync(bool enable);
		void SetTitle(std::string_view title);

		Ludus::Engine::Math::Size<int> GetFramebufferSize() const;
		Ludus::Engine::Math::Size<int> GetWindowSize() const;

		const WindowOptions GetWindowOptions() { return m_WindowOptions; }
		GLFWwindow* GetHandle() const { return m_Handle; }
	};
}
