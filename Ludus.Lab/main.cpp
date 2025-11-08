#include <Ludus/Core/Application.h>
#include <Ludus/Engine/Time.h>
#include <Ludus/Events/EventBus.h>
#include <Ludus/Events/EventHandler.h>
#include <Ludus/Events/EventType.h>
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
	auto eventBus = Ludus::Events::EventBus();
	auto input = Ludus::Platform::Input();
	auto time = Ludus::Engine::Time();
	auto window = Ludus::Platform::Window(windowOptions, eventBus);

	Ludus::Graphics::GLContext::Init();
	Ludus::Graphics::GLContext::EnableBlending();
	Ludus::Graphics::GLContext::SetBlendAlpha();

	eventBus.Subscribe(Ludus::Events::EventType::KeyEvent, (Ludus::Events::Eventhandler&)input);
	eventBus.Subscribe(Ludus::Events::EventType::TextInputEvent, (Ludus::Events::Eventhandler&)input);
	eventBus.Subscribe(Ludus::Events::EventType::MouseButtonEvent, (Ludus::Events::Eventhandler&)input);
	eventBus.Subscribe(Ludus::Events::EventType::MouseMoveEvent, (Ludus::Events::Eventhandler&)input);
	eventBus.Subscribe(Ludus::Events::EventType::MouseScrollEvent, (Ludus::Events::Eventhandler&)input);
	eventBus.Subscribe(Ludus::Events::EventType::WindowFocusEvent, (Ludus::Events::Eventhandler&)input);

	Ludus::Lab::AABBDemo2 demo2(window, WIDTH, HEIGHT);
	demo2.Init();

	while (!window.WindowShouldClose())
	{
		time.Step();
		input.Clear();

		window.PollEvents();

		if (input.GetKeyDown(Ludus::Platform::Key::Escape))
		{
			window.SetWindowShouldClose();
		}

		demo2.Update(time, input);

		window.SwapBuffers();
	}
}
