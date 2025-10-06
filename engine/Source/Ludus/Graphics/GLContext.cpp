#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Ludus/Graphics/GLContext.h>

namespace Ludus::Graphics
{
	using Ludus::Engine::Utilities::WriteLine;

	void GLContext::Init()
	{
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			WriteLine("Failed to initialize GLAD.");
			return;
		}

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(Ludus::Engine::Utilities::ErrorMessageCallback, nullptr);

		Ludus::Engine::Utilities::Write("GL version: ");
		WriteLine((const char*)glGetString(GL_VERSION));
	}

	void GLContext::EnableBlending()
	{
		glEnable(GL_BLEND);
	}

	void GLContext::SetBlendAlpha()
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}
