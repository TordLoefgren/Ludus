#include <pch.h>

#include <Ludus/Engine/Core/Application.h>
#include <Ludus/Engine/Core/ApplicationBuilder.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/RenderingOptions.h>
#include <Ludus/Engine/Platform/WindowOptions.h>
#include <Ludus/Lab/Core/Scene.h>
#include <Ludus/UI/Systems/ImGuiModule.h>

int main()
{
	auto windowOptions = Ludus::Engine::Platform::WindowOptions(1920, 1080, "Ludus Lab", true);
	auto renderingOptions = Ludus::Engine::Graphics::RenderingOptions(Ludus::Engine::Graphics::Colors::White);
	auto physicsContext = Ludus::Engine::Physics::Core::PhysicsConfiguration2D();

	auto applicationBuilder = Ludus::Engine::Core::ApplicationBuilder::Create();

	Ludus::UI::Systems::RegisterImGui(applicationBuilder);

	auto application = applicationBuilder
		.WithWindowOptions(windowOptions)
		.WithRenderingOptions(renderingOptions)
		.WithPhysicsConfiguration(std::move(physicsContext))
		.UseDefaultPhysics2D()
		.UseDefaultRendering2D()
		.AddUpdateSystem<Ludus::Lab::Core::Scene>()
		.Build();

	application->Run();
}
