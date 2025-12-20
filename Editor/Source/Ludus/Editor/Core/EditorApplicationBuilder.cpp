#include "pch.h"

#include <Ludus/Editor/Core/EditorApplicationBuilder.h>
#include <Ludus/Editor/Core/EditorConfiguration.h>
#include <Ludus/Editor/Core/EditorSystem.h>
#include <Ludus/Editor/Core/Scene.h>
#include <Ludus/Engine/Core/Application.h>
#include <Ludus/Engine/Core/ApplicationBuilder.h>
#include <Ludus/Engine/Graphics/RenderingConfiguration2D.h>
#include <Ludus/Engine/Graphics/RenderingOptions.h>
#include <Ludus/Engine/Physics/Core/PhysicsConfiguration2D.h>
#include <Ludus/Engine/Platform/WindowOptions.h>
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
		m_PhysicsConfiguration = Ludus::Engine::Physics::Core::PhysicsConfiguration2D();
		m_RenderingConfiguration = Ludus::Engine::Graphics::RenderingConfiguration2D();

		Ludus::UI::Systems::RegisterImGui(m_ApplicationBuilder);

		m_ApplicationBuilder
			.WithWindowOptions(m_WindowOptions)
			.WithRenderingOptions(m_RenderingOptions)
			.WithPhysicsConfiguration(std::move(m_PhysicsConfiguration))
			.WithRenderingConfiguration(std::move(m_RenderingConfiguration))
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
