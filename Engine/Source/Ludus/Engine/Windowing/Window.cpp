#include "pch.h"

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Debug/DebugGLFW.h>
#include <Ludus/Engine/Events/KeyboardEvents.h>
#include <Ludus/Engine/Events/MouseEvents.h>
#include <Ludus/Engine/Events/WindowEvents.h>
#include <Ludus/Engine/Graphics/GL.h>
#include <Ludus/Engine/Windowing/Window.h>

namespace Ludus::Engine::Windowing
{
	Window::Window(WindowOptions options, Ludus::Engine::Events::EventBus& eventBus)
		: m_Handle(nullptr),
		m_WindowOptions(options),
		m_EventBus(eventBus),
		m_UserData(&m_EventBus)
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

#if !defined(NDEBUG)
		// Subscribe to GLFW errors before initialization.
		glfwSetErrorCallback([](int code, const char* description)
		{
			Ludus::Engine::Debug::DebugGLFW::ErrorMessageCallback(code, description);
		});

#endif

		if (!glfwInit())
		{
			LUDUS_LOG_CRITICAL("Failed to initialize GLFW.");
			return;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, m_WindowOptions.IsResizeable ? GLFW_TRUE : GLFW_FALSE);

		m_Handle = glfwCreateWindow(m_WindowOptions.StartupWidth, m_WindowOptions.StartupHeight, m_WindowOptions.Title.data(), NULL, NULL);
		if (!m_Handle)
		{
			LUDUS_LOG_CRITICAL("Failed to create a GLFW window.");

			glfwTerminate();
			return;
		}

		if (!m_WindowOptions.IconPath.empty())
		{
			int width = 0;
			int height = 0;
			int channels = 0;
			auto* pixels = stbi_load(m_WindowOptions.IconPath.c_str(), &width, &height, &channels, 4);

			if (pixels)
			{
				GLFWimage image { };
				image.width = width;
				image.height = height;
				image.pixels = pixels;

				glfwSetWindowIcon(m_Handle, 1, &image);

				stbi_image_free(pixels);
			}
			else
			{
				LUDUS_LOG_WARN("Failed to load window icon: " + m_WindowOptions.IconPath);
			}
		}

		glfwMakeContextCurrent(m_Handle);
		glfwSwapInterval(1);

		glfwSetWindowUserPointer(m_Handle, &m_UserData);

		SetEventCallbacks();
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

	Ludus::Engine::Math::Size<int> Window::GetFramebufferSize() const
	{
		int width, height;
		glfwGetFramebufferSize(m_Handle, &width, &height);

		return { width, height };
	}

	Ludus::Engine::Math::Size<int> Window::GetWindowSize() const
	{
		int width, height;
		glfwGetWindowSize(m_Handle, &width, &height);

		return { width, height };
	}

	bool Window::WindowShouldClose()
	{
		return glfwWindowShouldClose(m_Handle);
	}

	void Window::SetVSync(bool enable)
	{
		glfwSwapInterval(enable ? 1 : 0);
	}

	void Window::SetTitle(std::string_view title)
	{
		glfwSetWindowTitle(m_Handle, std::string(title).c_str());
	}

	void Window::SetEventCallbacks()
	{
		// Keyboard and Text callbacks.

		glfwSetKeyCallback(m_Handle, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			auto& data = *(WindowUserData*)glfwGetWindowUserPointer(window);
			data.EventBus->PublishEvent(Ludus::Engine::Events::KeyboardEvents::KeyEvent(key, scancode, action, mods));
		});

		glfwSetCharCallback(m_Handle, [](GLFWwindow* window, unsigned int codepoint)
		{
			auto& data = *(WindowUserData*)glfwGetWindowUserPointer(window);
			data.EventBus->PublishEvent(Ludus::Engine::Events::KeyboardEvents::TextInputEvent(codepoint));
		});

		// Mouse callbacks.

		glfwSetMouseButtonCallback(m_Handle, [](GLFWwindow* window, int button, int action, int mods)
		{
			auto& data = *(WindowUserData*)glfwGetWindowUserPointer(window);
			data.EventBus->PublishEvent(Ludus::Engine::Events::MouseEvents::MouseButtonEvent(button, action, mods));
		});

		glfwSetCursorPosCallback(m_Handle, [](GLFWwindow* window, double xPosition, double yPosition)
		{
			auto& data = *(WindowUserData*)glfwGetWindowUserPointer(window);
			data.EventBus->PublishEvent(Ludus::Engine::Events::MouseEvents::MouseMoveEvent(xPosition, yPosition));
		});

		glfwSetScrollCallback(m_Handle, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			auto& data = *(WindowUserData*)glfwGetWindowUserPointer(window);
			data.EventBus->PublishEvent(Ludus::Engine::Events::MouseEvents::MouseScrollEvent(xOffset, yOffset));
		});

		glfwSetCursorEnterCallback(m_Handle, [](GLFWwindow* window, int entered)
		{
			auto& data = *(WindowUserData*)glfwGetWindowUserPointer(window);
			data.EventBus->PublishEvent(Ludus::Engine::Events::MouseEvents::MouseEnterEvent(entered == GLFW_TRUE));
		});

		// Window callbacks.

		glfwSetFramebufferSizeCallback(m_Handle, [](GLFWwindow* window, int width, int height)
		{
			auto& data = *(WindowUserData*)glfwGetWindowUserPointer(window);
			data.EventBus->PublishEvent(Ludus::Engine::Events::WindowEvents::FramebufferSizeEvent(width, height));
		});

		glfwSetWindowCloseCallback(m_Handle, [](GLFWwindow* window)
		{
			auto& data = *(WindowUserData*)glfwGetWindowUserPointer(window);

			// Let the actual event close the window.
			glfwSetWindowShouldClose(window, GLFW_FALSE);

			data.EventBus->PublishEvent(Ludus::Engine::Events::WindowEvents::WindowCloseEvent());
		});

		glfwSetWindowPosCallback(m_Handle, [](GLFWwindow* window, int xPosition, int yPosition)
		{
			auto& data = *(WindowUserData*)glfwGetWindowUserPointer(window);
			data.EventBus->PublishEvent(Ludus::Engine::Events::WindowEvents::WindowPositionEvent(xPosition, yPosition));
		});

		glfwSetWindowSizeCallback(m_Handle, [](GLFWwindow* window, int width, int height)
		{
			auto& data = *(WindowUserData*)glfwGetWindowUserPointer(window);

			data.EventBus->PublishEvent(Ludus::Engine::Events::WindowEvents::WindowSizeEvent(width, height));
		});

		glfwSetWindowFocusCallback(m_Handle, [](GLFWwindow* window, int focused)
		{
			auto& data = *(WindowUserData*)glfwGetWindowUserPointer(window);

			data.EventBus->PublishEvent(Ludus::Engine::Events::WindowEvents::WindowFocusEvent(focused == GLFW_TRUE));
		});

		glfwSetWindowIconifyCallback(m_Handle, [](GLFWwindow* window, int iconified)
		{
			auto& data = *(WindowUserData*)glfwGetWindowUserPointer(window);

			data.EventBus->PublishEvent(Ludus::Engine::Events::WindowEvents::WindowIconifyEvent(iconified == GLFW_TRUE));
		});

		glfwSetWindowMaximizeCallback(m_Handle, [](GLFWwindow* window, int maximized)
		{
			auto& data = *(WindowUserData*)glfwGetWindowUserPointer(window);

			data.EventBus->PublishEvent(Ludus::Engine::Events::WindowEvents::WindowMaximizeEvent(maximized == GLFW_TRUE));
		});

		glfwSetWindowContentScaleCallback(m_Handle, [](GLFWwindow* window, float xScale, float yScale)
		{
			auto& data = *(WindowUserData*)glfwGetWindowUserPointer(window);
			data.EventBus->PublishEvent(Ludus::Engine::Events::WindowEvents::WindowContentScaleEvent(xScale, yScale));
		});
	}
}
