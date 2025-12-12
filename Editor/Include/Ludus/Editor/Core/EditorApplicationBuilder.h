#pragma once

#include <memory>

#include <Ludus/Editor/Core/Scene.h>
#include <Ludus/Editor/Panels/ConsolePanel.h>
#include <Ludus/Editor/Panels/DockPanel.h>
#include <Ludus/Editor/Panels/ImGuiDemoPanel.h>
#include <Ludus/Editor/Panels/InspectorPanel.h>
#include <Ludus/Editor/Panels/SceneHierarchyPanel.h>
#include <Ludus/Editor/Panels/ViewportPanel.h>
#include <Ludus/Engine/Core/Application.h>
#include <Ludus/Engine/Core/ApplicationBuilder.h>
#include <Ludus/Engine/Graphics/Camera2D.h>
#include <Ludus/Engine/Graphics/RenderingOptions.h>
#include <Ludus/Engine/Physics/Core/PhysicsContext2D.h>
#include <Ludus/Engine/Platform/WindowOptions.h>
#include <Ludus/UI/Utilities.h>

namespace Ludus::Editor::Core
{
	class EditorApplicationBuilder
	{
	private:
		Ludus::Engine::Core::ApplicationBuilder m_ApplicationBuilder;

		Ludus::Engine::Platform::WindowOptions m_WindowOptions;
		Ludus::Engine::Graphics::RenderingOptions m_RenderingOptions;
		Ludus::Engine::Physics::Core::PhysicsContext2D m_PhysicsContext;

	public:
		static EditorApplicationBuilder Create();

		std::unique_ptr<Ludus::Engine::Core::Application> Build();

		EditorApplicationBuilder& AddDefaultEngine();
		EditorApplicationBuilder& AddEditorPanels();
		EditorApplicationBuilder& AddEditorViewport(std::string title = "Viewport", std::shared_ptr<Ludus::Engine::Graphics::Camera2D> camera = nullptr);
	};
}
