#include "GLContext.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void GLContext::Init()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		WriteLine("Failed to initialize GLAD.");
		return;
	}

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(ErrorMessageCallback, nullptr);

	Write("GL version: ");
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
