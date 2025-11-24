#include <Ludus/Core/Application.h>
#include <Ludus/Core/Phase.h>
#include <Ludus/Core/State.h>
#include <Ludus/Core/SystemPredicate.h>
#include <Ludus/Platform/WindowOptions.h>

#include <Lab/Core/Scene.h>

int main()
{
	auto windowOptions = Ludus::Platform::WindowOptions(1024, 768, "Ludus Lab", false);
	auto application = Ludus::Core::Application::Create(windowOptions);

	application->AddSystem(Ludus::Core::Phase::Update, std::make_unique<Ludus::Lab::Core::Scene>());
	application->Run();
}
