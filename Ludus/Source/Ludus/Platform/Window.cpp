#include <glad/glad.h>

#include <Ludus/Platform/Window.h>

namespace Ludus::Platform
{
	using Ludus::Engine::Utilities::WriteLine;

	Window::Window(WindowOptions options)
		: m_Width(options.Width), m_Height(options.Height), m_Title(options.Title), m_Resizable(options.resizeable), m_Handle(nullptr), m_Input()
	{
		Setup();
	}

	Window::Window(int width, int height, std::string title, bool resizeable)
		: m_Width(width), m_Height(height), m_Title(title), m_Resizable(resizeable), m_Handle(nullptr), m_Input()
	{
		Setup();
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_Handle);
		glfwTerminate();
	}

	void Window::Setup()
	{
		if (!glfwInit())
		{
			WriteLine("Failed to initialize GLFW.");
			return;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, m_Resizable ? GLFW_TRUE : GLFW_FALSE);

		m_Handle = glfwCreateWindow(m_Width, m_Height, m_Title.data(), NULL, NULL);
		if (!m_Handle)
		{
			WriteLine("Failed to create a GLFW window.");
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

	void Window::SetWindowShouldClose()
	{
		glfwSetWindowShouldClose(m_Handle, GLFW_TRUE);
	}

	void Window::SwapBuffers()
	{
		glfwSwapBuffers(m_Handle);
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
