#pragma once

#include <memory>

#include <Ludus/Editor/Core/EditorConfiguration.h>
#include <Ludus/Engine/Core/Application.h>
#include <Ludus/Engine/Core/ApplicationBuilder.h>
#include <Ludus/Engine/Core/ApplicationOptions.h>
#include <Ludus/Engine/Graphics/RenderingConfiguration2D.h>
#include <Ludus/Engine/Graphics/RenderingOptions.h>
#include <Ludus/Engine/Physics/Core/PhysicsConfiguration2D.h>
#include <Ludus/Engine/Platform/WindowOptions.h>

namespace Ludus::Editor::Core
{
	class EditorApplicationBuilder
	{
	private:
		Ludus::Engine::Core::ApplicationBuilder m_ApplicationBuilder;

		Ludus::Engine::Core::ApplicationOptions m_ApplicationOptions;
		Ludus::Engine::Graphics::RenderingConfiguration2D m_RenderingConfiguration;
		Ludus::Engine::Graphics::RenderingOptions m_RenderingOptions;
		Ludus::Engine::Physics::Core::PhysicsConfiguration2D m_PhysicsConfiguration;
		Ludus::Engine::Platform::WindowOptions m_WindowOptions;

		Ludus::Editor::Core::EditorConfiguration m_EditorConfiguration = Ludus::Editor::Core::EditorConfiguration::Default();

	public:
		static EditorApplicationBuilder Create();

		std::unique_ptr<Ludus::Engine::Core::Application> Build();

		EditorApplicationBuilder& WithEditorConfiguration(Ludus::Editor::Core::EditorConfiguration editorConfiguration);

		EditorApplicationBuilder& AddDefaultEngine();
		EditorApplicationBuilder& AddEditorSystem();
	};
}
