#include <Ludus/Core/Application.h>
#include <Ludus/Core/Phase.h>
#include <Ludus/Core/State.h>
#include <Ludus/Core/SystemPredicate.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/RenderingOptions.h>
#include <Ludus/Platform/WindowOptions.h>

#include <Lab/Core/Scene.h>

int main()
{
	auto windowOptions = Ludus::Platform::WindowOptions(1024, 768, "Ludus Lab", false);
	auto renderingOptions = Ludus::Graphics::RenderingOptions(Ludus::Graphics::Colors::White);
	auto application = Ludus::Core::Application::Create(windowOptions, renderingOptions);

	application->AddSystem(Ludus::Core::Phase::Update, std::make_unique<Ludus::Lab::Core::Scene>());
	application->Run();
}
