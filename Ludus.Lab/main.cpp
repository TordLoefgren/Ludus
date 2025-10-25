#include <Ludus/Core/Application.h>
#include <Ludus/Graphics/GLContext.h>
#include <Ludus/Platform/Input.h>
#include <Ludus/Platform/Window.h>
#include <Ludus/Platform/WindowOptions.h>

#include <AABBDemo1.h>
#include <AABBDemo2.h>
#include <IDemo.h>

const int WIDTH = 1024;
const int HEIGHT = 640;

int main()
{
	auto windowOptions = Ludus::Platform::WindowOptions(WIDTH, HEIGHT, "Ludus Lab", false);
	auto input = Ludus::Platform::Input();
	auto window = Ludus::Platform::Window(windowOptions, input);

	Ludus::Graphics::GLContext::Init();
	Ludus::Graphics::GLContext::EnableBlending();
	Ludus::Graphics::GLContext::SetBlendAlpha();

	Ludus::Lab::AABBDemo2 demo2(window, WIDTH, HEIGHT);
	demo2.Init();

	while (!window.WindowShouldClose())
	{
		window.PollEvents();

		if (input.GetKeyDown(Ludus::Platform::Key::Escape))
		{
			window.SetWindowShouldClose();
		}

		demo2.Update();

		window.SwapBuffers();
	}
}
