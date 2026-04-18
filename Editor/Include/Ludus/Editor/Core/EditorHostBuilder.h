#pragma once

#include <memory>
#include <optional>

#include <Ludus/Editor/Core/EditorConfiguration.h>
#include <Ludus/Editor/Core/EditorPreferences.h>
#include <Ludus/Editor/Core/EditorStartupOptions.h>
#include <Ludus/Editor/Persistence/EditorPersistence.h>
#include <Ludus/Engine/Runtime/ApplicationHostBuilder.h>
#include <Ludus/Engine/Runtime/RuntimeOptions.h>
#include <Ludus/Engine/Windowing/WindowOptions.h>

namespace Ludus::Editor::Core
{
	class EditorHostBuilder
	{
	private:
		Ludus::Engine::Runtime::ApplicationHostBuilder m_ApplicationHostBuilder;
		Ludus::Editor::Core::EditorConfiguration m_EditorConfiguration = Ludus::Editor::Core::EditorConfiguration::Default();
		Ludus::Editor::Persistence::EditorPersistence m_EditorPersistence = Ludus::Editor::Persistence::EditorPersistence::DefaultText();
		Ludus::Editor::Core::EditorPreferences m_EditorPreferences = Ludus::Editor::Core::EditorPreferences::Default();
		Ludus::Editor::Core::EditorStartupOptions m_EditorStartupOptions = Ludus::Editor::Core::EditorStartupOptions::Default();

		std::optional<Ludus::Engine::Runtime::RuntimeOptions> m_RuntimeOptions;
		std::optional<Ludus::Engine::Windowing::WindowOptions> m_WindowOptions;

		bool m_UseEditor = false;
		bool m_UseImGui = false;

	public:
		static EditorHostBuilder Create();

		std::unique_ptr<Ludus::Engine::Runtime::ApplicationHost> Build();

		EditorHostBuilder& WithEditorConfiguration(Ludus::Editor::Core::EditorConfiguration editorConfiguration);
		EditorHostBuilder& WithEditorPersistence(Ludus::Editor::Persistence::EditorPersistence editorPersistence);
		EditorHostBuilder& WithEditorPreferences(Ludus::Editor::Core::EditorPreferences editorPreferences);
		EditorHostBuilder& WithEditorStartupOptions(EditorStartupOptions startupOptions);
		EditorHostBuilder& WithRuntimeOptions(Ludus::Engine::Runtime::RuntimeOptions runtimeOptions);
		EditorHostBuilder& WithWindowOptions(Ludus::Engine::Windowing::WindowOptions windowOptions);

		EditorHostBuilder& UseEditor();
		EditorHostBuilder& UseEditorHostDefaults();
	};
}
