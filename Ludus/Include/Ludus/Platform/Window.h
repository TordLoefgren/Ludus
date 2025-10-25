#pragma once

#include <GLFW/glfw3.h>

#include <Ludus/Engine/Utilities.h>
#include <Ludus/Platform/Input.h>
#include <Ludus/Platform/WindowOptions.h>

namespace Ludus::Platform
{
	class Window
	{
	private:
		GLFWwindow* m_Handle;
		WindowOptions m_WindowOptions;
		Input& m_Input;

		void Init();

	public:
		Window(WindowOptions options, Input& input);
		~Window();

		void PollEvents();
		void SetWindowShouldClose();
		void SwapBuffers();
		bool WindowShouldClose();

		Input& GetInput();
	};
}
