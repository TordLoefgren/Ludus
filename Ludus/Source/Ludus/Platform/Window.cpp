#include <glad/glad.h>

#include <Ludus/Debug/Debug.h>
#include <Ludus/Platform/Window.h>

namespace Ludus::Platform
{
	Window::Window(WindowOptions options, Input& input)
		: m_Handle(nullptr),
		m_WindowOptions(options),
		m_Input(input)
	{
		Init();
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_Handle);
		glfwTerminate();
	}

	void Window::Init()
	{
		if (!glfwInit())
		{
			LUDUS_LOG_CRITICAL("Failed to initialize GLFW.");
			return;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, m_WindowOptions.IsResizeable ? GLFW_TRUE : GLFW_FALSE);

		m_Handle = glfwCreateWindow(m_WindowOptions.Width, m_WindowOptions.Height, m_WindowOptions.Title.data(), NULL, NULL);
		if (!m_Handle)
		{
			LUDUS_LOG_CRITICAL("Failed to create a GLFW window.");

			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(m_Handle);
		glfwSwapInterval(1);

		m_Input.SetHandle(m_Handle);
	}

	void Window::PollEvents()
	{
		glfwPollEvents();
	}

	void Window::SwapBuffers()
	{
		glfwSwapBuffers(m_Handle);
	}

	void Window::SetWindowShouldClose()
	{
		glfwSetWindowShouldClose(m_Handle, GLFW_TRUE);
	}

	bool Window::WindowShouldClose()
	{
		return glfwWindowShouldClose(m_Handle);
	}

	Input& Window::GetInput()
	{
		return m_Input;
	}
}
