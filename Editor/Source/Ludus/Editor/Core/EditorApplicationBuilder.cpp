#include "pch.h"

#include <Ludus/Editor/Core/EditorApplicationBuilder.h>
#include <Ludus/Editor/Core/EditorConfiguration.h>
#include <Ludus/Editor/Core/EditorSystem.h>
#include <Ludus/Editor/Core/Scene.h>
#include <Ludus/UI/Systems/ImGuiModule.h>

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

	EditorApplicationBuilder& EditorApplicationBuilder::WithEditorConfiguration(Ludus::Editor::Core::EditorConfiguration editorConfiguration)
	{
		m_EditorConfiguration = editorConfiguration;

		return *this;
	}

	EditorApplicationBuilder& EditorApplicationBuilder::AddDefaultEngine()
	{
		m_WindowOptions = Ludus::Engine::Platform::WindowOptions(1920, 1080, "Ludus Editor", true);
		m_RenderingOptions = Ludus::Engine::Graphics::RenderingOptions(Ludus::Engine::Graphics::Colors::DarkGray);
		m_PhysicsContext = Ludus::Engine::Physics::Core::PhysicsContext2D();

		Ludus::UI::Systems::RegisterImGui(m_ApplicationBuilder);

		m_ApplicationBuilder
			.WithWindowOptions(m_WindowOptions)
			.WithRenderingOptions(m_RenderingOptions)
			.WithPhysicsContext(std::move(m_PhysicsContext))
			.UseDefaultPhysics2D()
			.UseDefaultRendering2D();

		return *this;
	}

	EditorApplicationBuilder& EditorApplicationBuilder::AddEditorSystem()
	{
		m_ApplicationBuilder
			.AddUpdateSystem<Ludus::Editor::Core::EditorSystem>(m_EditorConfiguration)
			.AddUpdateSystem<Ludus::Editor::Core::Scene>();

		return *this;
	}
}
