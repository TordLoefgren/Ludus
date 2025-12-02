#include <Ludus/Core/Application.h>
#include <Ludus/Core/SystemPhase.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/RenderingOptions.h>
#include <Ludus/Platform/WindowOptions.h>

#include <Ludus/Lab/Core/Scene.h>

int main()
{
	auto windowOptions = Ludus::Platform::WindowOptions(1600, 900, "Ludus Lab", true);
	auto renderingOptions = Ludus::Graphics::RenderingOptions(Ludus::Graphics::Colors::White);
	auto application = Ludus::Core::Application::Create(windowOptions, renderingOptions);

	application->AddSystem(
		{ Ludus::Core::SystemPhase::Update },
		std::make_unique<Ludus::Lab::Core::Scene>()
	);

	application->Run();
}
