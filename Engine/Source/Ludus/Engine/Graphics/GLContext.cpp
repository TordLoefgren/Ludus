#include "pch.h"

#include <format>

#include <GLFW/glfw3.h>

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Debug/DebugGL.h>
#include <Ludus/Engine/Events/WindowEvents.h>
#include <Ludus/Engine/Graphics/GL.h>
#include <Ludus/Engine/Graphics/GLContext.h>

namespace Ludus::Engine::Graphics
{
	void GLContext::Init()
	{
		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		{
			LUDUS_LOG_CRITICAL("Failed to initialize GLAD.");

			return;
		}

		Ludus::Engine::Debug::EnableOpenGLDebug();

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

	bool GLContext::ProcessEvent(const Ludus::Engine::Events::Event& event)
	{
		using EventType = Ludus::Engine::Events::EventType;

		switch (event.Type)
		{
			case EventType::FramebufferSizeEvent:
			{
				const auto& e = static_cast<const Ludus::Engine::Events::WindowEvents::FramebufferSizeEvent&>(event);
				SetViewport(e.Width, e.Height);
				return true;
			}

			default: return false;
		}

		return false;
	}
}
