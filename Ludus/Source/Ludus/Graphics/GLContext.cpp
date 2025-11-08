#include <format>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Ludus/Debug/Debug.h>
#include <Ludus/Debug/DebugGL.h>
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

	bool GLContext::ProcessEvent(const Ludus::Events::Event& event)
	{
		// Not yet implemented. 

		return false;
	}
}
