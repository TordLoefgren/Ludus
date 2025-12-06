#include <Ludus/Engine/Core/Application.h>
#include <Ludus/Engine/Core/SystemPhase.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/RenderingOptions.h>
#include <Ludus/Engine/Platform/WindowOptions.h>

#include <Ludus/Lab/Core/Scene.h>

int main()
{
	auto windowOptions = Ludus::Engine::Platform::WindowOptions(1600, 900, "Ludus Lab", true);
	auto renderingOptions = Ludus::Engine::Graphics::RenderingOptions(Ludus::Engine::Graphics::Colors::White);
	auto application = Ludus::Engine::Core::Application::Create(windowOptions, renderingOptions);

	application->AddSystem(
		{ Ludus::Engine::Core::SystemPhase::Update },
		std::make_unique<Ludus::Lab::Core::Scene>()
	);

	application->Run();
}
