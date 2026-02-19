#include "pch.h"

#include <Ludus/Editor/Core/EditorApplicationBuilder.h>
#include <Ludus/Editor/Core/EditorConfiguration.h>
#include <Ludus/Editor/Core/EditorExecutionFlags.h>
#include <Ludus/Editor/Core/EditorGridRenderPass.h>
#include <Ludus/Editor/Core/EditorSystem.h>
#include <Ludus/Engine/Core/Application.h>
#include <Ludus/Engine/Core/ApplicationBuilder.h>
#include <Ludus/Engine/Core/ApplicationOptions.h>
#include <Ludus/Engine/Graphics/RenderingConfiguration2D.h>
#include <Ludus/Engine/Graphics/RenderingOptions.h>
#include <Ludus/Engine/Physics/Core/PhysicsConfiguration2D.h>
#include <Ludus/Engine/Windowing/WindowOptions.h>
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
		m_ApplicationOptions = Ludus::Engine::Core::ApplicationOptions(Ludus::Editor::Core::DefaultEditorExecutionMask);
		m_RenderingConfiguration = Ludus::Engine::Graphics::RenderingConfiguration2D();
		m_RenderingConfiguration.AddPass(std::make_unique<EditorGridRenderPass>());
		m_RenderingOptions = Ludus::Engine::Graphics::RenderingOptions(Ludus::Engine::Graphics::Colors::DarkGray);
		m_RenderViewConfiguration = Ludus::Engine::Graphics::RenderViewConfiguration(false);
		m_PhysicsConfiguration = Ludus::Engine::Physics::Core::PhysicsConfiguration2D();
		m_WindowOptions = Ludus::Engine::Windowing::WindowOptions(1920, 1080, "Ludus Editor", true, "Resources/LudusIcon.png");

		Ludus::UI::Systems::RegisterImGui(m_ApplicationBuilder);

		m_ApplicationBuilder
			.WithApplicationOptions(m_ApplicationOptions)
			.WithRenderingConfiguration(std::move(m_RenderingConfiguration))
			.WithRenderingOptions(m_RenderingOptions)
			.WithRenderViewConfiguration(m_RenderViewConfiguration)
			.WithPhysicsConfiguration(std::move(m_PhysicsConfiguration))
			.WithWindowOptions(m_WindowOptions)
			.UseDefaultPhysics2D()
			.UseDefaultRendering2D();

		return *this;
	}

	EditorApplicationBuilder& EditorApplicationBuilder::AddEditorSystem()
	{
		m_ApplicationBuilder
			.AddUpdateSystem<Ludus::Editor::Core::EditorSystem>(m_EditorConfiguration);

		return *this;
	}
}
