#include <memory>

#include <Ludus/Editor/Core/Scene.h>
#include <Ludus/Editor/Panels/ConsolePanel.h>
#include <Ludus/Editor/Panels/DockPanel.h>
#include <Ludus/Editor/Panels/ImGuiDemoPanel.h>
#include <Ludus/Editor/Panels/InspectorPanel.h>
#include <Ludus/Editor/Panels/SceneHierarchyPanel.h>
#include <Ludus/Editor/Panels/ViewportPanel.h>
#include <Ludus/Engine/Core/Application.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/RenderingOptions.h>
#include <Ludus/Engine/Graphics/RenderTarget.h>
#include <Ludus/Engine/Platform/WindowOptions.h>

int main()
{
	auto windowOptions = Ludus::Engine::Platform::WindowOptions(1600, 900, "Ludus Editor", true);
	auto renderingOptions = Ludus::Engine::Graphics::RenderingOptions(Ludus::Engine::Graphics::Colors::White);
	auto application = Ludus::Engine::Core::Application::Create(windowOptions, renderingOptions);

	application->AddResource<std::shared_ptr<Ludus::Engine::Graphics::RenderTarget>>(std::make_shared<Ludus::Engine::Graphics::RenderTarget>("Viewport", windowOptions.Width, windowOptions.Height));

	application->AddSystem(
		{ { Ludus::Engine::Core::SystemPhase::Update } },
		std::make_unique<Ludus::Editor::Panels::DockPanel>()
	);

	application->AddSystem(
		{ { Ludus::Engine::Core::SystemPhase::Update } },
		std::make_unique<Ludus::Editor::Panels::ConsolePanel>()
	);

	application->AddSystem(
		{ { Ludus::Engine::Core::SystemPhase::Update } },
		std::make_unique<Ludus::Editor::Panels::ImGuiDemoPanel>()
	);

	application->AddSystem(
		{ { Ludus::Engine::Core::SystemPhase::Update } },
		std::make_unique<Ludus::Editor::Panels::InspectorPanel>()
	);

	application->AddSystem(
		{ { Ludus::Engine::Core::SystemPhase::Update } },
		std::make_unique<Ludus::Editor::Panels::SceneHierarchyPanel>()
	);

	application->AddSystem(
		{ { Ludus::Engine::Core::SystemPhase::Update } },
		std::make_unique<Ludus::Editor::Panels::ViewportPanel>()
	);

	application->AddSystem(
		{ { Ludus::Engine::Core::SystemPhase::Update } },
		std::make_unique<Ludus::Editor::Core::Scene>()
	);

	application->Run();
}
