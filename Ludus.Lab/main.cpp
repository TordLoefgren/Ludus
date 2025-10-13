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
	auto window = Ludus::Platform::Window(windowOptions);

	Ludus::Graphics::GLContext::Init();
	Ludus::Graphics::GLContext::EnableBlending();
	Ludus::Graphics::GLContext::SetBlendAlpha();

	Ludus::Lab::AABBDemo2 demo2(window, WIDTH, HEIGHT);
	demo2.Init();

	while (!window.WindowShouldClose())
	{
		demo2.Update();

		window.SwapBuffers();
		window.PollEvents();
	}
}
