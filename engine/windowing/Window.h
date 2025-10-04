#pragma once

#include "Input.h"
#include "WindowOptions.h"

struct GLFWwindow;

class Window
{
private:
	GLFWwindow* m_Handle;
	Input m_Input;
	int m_Width, m_Height;
	std::string m_Title;
	bool m_Resizable;

	void Setup();

public:
	Window(WindowOptions options);
	Window(int width, int height, std::string title, bool resizeable = true);
	~Window();

	void PollEvents();
	void SetWindowShouldClose();
	void SwapBuffers();
	bool WindowShouldClose();

	Input& GetInput();
};
