#include <format>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Ludus/Debug/Debug.h>
#include <Ludus/Debug/DebugGL.h>
#include <Ludus/Events/WindowEvents.h>
#include <Ludus/Graphics/GLContext.h>

namespace Ludus::Graphics
{
	void GLContext::Init()
	{
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			LUDUS_LOG_CRITICAL("Failed to initialize GLAD.");

			return;
		}

		Ludus::Debug::EnableOpenGLDebug();

		auto* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
		if (version)
		{
			LUDUS_LOG_INFO(
				std::format(
					"GL version: {}",
					version
				)
			);
		}
		else
		{
			LUDUS_LOG_ERROR("Failed to determine OpenGL version.");
		}
	}

	void GLContext::EnableBlending()
	{
		glEnable(GL_BLEND);
	}

	void GLContext::SetBlendAlpha()
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void GLContext::SetViewport(int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	bool GLContext::ProcessEvent(const Ludus::Events::Event& event)
	{
		using EventType = Ludus::Events::EventType;

		switch (event.Type)
		{
			case EventType::FramebufferSizeEvent:
			{
				const auto& e = static_cast<const Ludus::Events::WindowEvents::FramebufferSizeEvent&>(event);
				SetViewport(e.Width, e.Height);
				return true;
			}

			default: return false;
		}

		return false;
	}
}
