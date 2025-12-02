#include <memory>

#include <Ludus/Core/Application.h>
#include <Ludus/Editor/Core/Scene.h>
#include <Ludus/Editor/Panels/ConsolePanel.h>
#include <Ludus/Editor/Panels/DockPanel.h>
#include <Ludus/Editor/Panels/ImGuiDemoPanel.h>
#include <Ludus/Editor/Panels/InspectorPanel.h>
#include <Ludus/Editor/Panels/SceneHierarchyPanel.h>
#include <Ludus/Editor/Panels/ViewportPanel.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/RenderingOptions.h>
#include <Ludus/Graphics/RenderTarget.h>
#include <Ludus/Platform/WindowOptions.h>

int main()
{
	auto windowOptions = Ludus::Platform::WindowOptions(1600, 900, "Ludus Editor", true);
	auto renderingOptions = Ludus::Graphics::RenderingOptions(Ludus::Graphics::Colors::White);
	auto application = Ludus::Core::Application::Create(windowOptions, renderingOptions);

	application->AddResource<std::shared_ptr<Ludus::Graphics::RenderTarget>>(std::make_shared<Ludus::Graphics::RenderTarget>("Viewport", windowOptions.Width, windowOptions.Height));

	application->AddSystem(
		{ { Ludus::Core::SystemPhase::Update } },
		std::make_unique<Ludus::Editor::Panels::DockPanel>()
	);

	application->AddSystem(
		{ { Ludus::Core::SystemPhase::Update } },
		std::make_unique<Ludus::Editor::Panels::ConsolePanel>()
	);

	application->AddSystem(
		{ { Ludus::Core::SystemPhase::Update } },
		std::make_unique<Ludus::Editor::Panels::ImGuiDemoPanel>()
	);

	application->AddSystem(
		{ { Ludus::Core::SystemPhase::Update } },
		std::make_unique<Ludus::Editor::Panels::InspectorPanel>()
	);

	application->AddSystem(
		{ { Ludus::Core::SystemPhase::Update } },
		std::make_unique<Ludus::Editor::Panels::SceneHierarchyPanel>()
	);

	application->AddSystem(
		{ { Ludus::Core::SystemPhase::Update } },
		std::make_unique<Ludus::Editor::Panels::ViewportPanel>()
	);

	application->AddSystem(
		{ { Ludus::Core::SystemPhase::Update } },
		std::make_unique<Ludus::Editor::Core::Scene>()
	);

	application->Run();
}
