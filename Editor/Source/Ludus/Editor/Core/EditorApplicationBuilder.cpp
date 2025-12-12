#include "pch.h"

#include <Ludus/Editor/Core/EditorApplicationBuilder.h>

namespace Ludus::Editor::Core
{
	std::unique_ptr<Ludus::Engine::Core::Application> EditorApplicationBuilder::Build()
	{
		return m_ApplicationBuilder.Build();
	}

	EditorApplicationBuilder EditorApplicationBuilder::Create()
	{
		return EditorApplicationBuilder();
	}

	EditorApplicationBuilder& EditorApplicationBuilder::AddDefaultEngine()
	{
		m_WindowOptions = Ludus::Engine::Platform::WindowOptions(1920, 1080, "Ludus Editor", true);
		m_RenderingOptions = Ludus::Engine::Graphics::RenderingOptions(Ludus::Engine::Graphics::Colors::White);
		m_PhysicsContext = Ludus::Engine::Physics::Core::PhysicsContext2D();

		Ludus::UI::Utilities::UseImGui(m_ApplicationBuilder);

		m_ApplicationBuilder
			.WithWindowOptions(m_WindowOptions)
			.WithRenderingOptions(m_RenderingOptions)
			.WithPhysicsContext(std::move(m_PhysicsContext))
			.UseDefaultPhysics2D()
			.UseDefaultRendering2D();

		return *this;
	}

	EditorApplicationBuilder& EditorApplicationBuilder::AddEditorPanels()
	{
		m_ApplicationBuilder
			.AddUpdateSystem<Ludus::Editor::Panels::DockPanel>()
			.AddUpdateSystem<Ludus::Editor::Panels::ConsolePanel>()
			.AddUpdateSystem<Ludus::Editor::Panels::ImGuiDemoPanel>()
			.AddUpdateSystem<Ludus::Editor::Panels::InspectorPanel>()
			.AddUpdateSystem<Ludus::Editor::Panels::SceneHierarchyPanel>()
			.AddUpdateSystem<Ludus::Editor::Core::Scene>();

		return *this;
	}

	EditorApplicationBuilder& EditorApplicationBuilder::AddEditorViewport(std::string title, std::shared_ptr<Ludus::Engine::Graphics::Camera2D> camera)
	{
		m_ApplicationBuilder.Configure(
			[title = title, camera = camera](Ludus::Engine::Core::Application& application)
			{
				auto viewportPanel = std::make_unique<Ludus::Editor::Panels::ViewportPanel>(title, std::move(camera));
				application.AddSystem({ Ludus::Engine::Core::SystemPhase::Update }, std::move(viewportPanel));
			}
		);

		return *this;
	}
}
