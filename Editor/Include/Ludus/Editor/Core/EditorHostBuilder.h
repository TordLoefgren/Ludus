#pragma once

#include <memory>

#include <Ludus/Editor/Core/EditorConfiguration.h>
#include <Ludus/Engine/Graphics/RenderingConfiguration2D.h>
#include <Ludus/Engine/Graphics/RenderingOptions.h>
#include <Ludus/Engine/Graphics/RenderViewConfiguration.h>
#include <Ludus/Engine/Physics/Core/PhysicsConfiguration2D.h>
#include <Ludus/Engine/Runtime/ApplicationHostBuilder.h>
#include <Ludus/Engine/Runtime/RuntimeOptions.h>
#include <Ludus/Engine/Windowing/WindowOptions.h>

namespace Ludus::Editor::Core
{
	class EditorHostBuilder
	{
	private:
		Ludus::Engine::Runtime::ApplicationHostBuilder m_ApplicationHostBuilder;

		Ludus::Engine::Graphics::RenderingConfiguration2D m_RenderingConfiguration;
		Ludus::Engine::Graphics::RenderingOptions m_RenderingOptions;
		Ludus::Engine::Graphics::RenderViewConfiguration m_RenderViewConfiguration;
		Ludus::Engine::Runtime::RuntimeOptions m_RuntimeOptions;
		Ludus::Engine::Physics::Core::PhysicsConfiguration2D m_PhysicsConfiguration;
		Ludus::Engine::Windowing::WindowOptions m_WindowOptions;

		Ludus::Editor::Core::EditorConfiguration m_EditorConfiguration = Ludus::Editor::Core::EditorConfiguration::Default();

	public:
		static EditorHostBuilder Create();

		std::unique_ptr<Ludus::Engine::Runtime::ApplicationHost> Build();

		EditorHostBuilder& WithEditorConfiguration(Ludus::Editor::Core::EditorConfiguration editorConfiguration);

		EditorHostBuilder& AddDefaultEngine();
		EditorHostBuilder& AddEditorSystem();
	};
}
