#pragma once

#include <optional>

#include <Ludus/Editor/Core/EditorConfiguration.h>
#include <Ludus/Editor/Core/EditorPersistenceContext.h>
#include <Ludus/Editor/Core/EditorPreferences.h>
#include <Ludus/Editor/Core/EditorSession.h>
#include <Ludus/Editor/Core/EditorShell.h>
#include <Ludus/Editor/Core/EditorStartupOptions.h>
#include <Ludus/Editor/Core/ProjectSession.h>
#include <Ludus/Editor/Core/WelcomeWindow.h>
#include <Ludus/Editor/Panels/PanelRegistry.h>
#include <Ludus/Editor/Persistence/EditorPersistence.h>
#include <Ludus/Editor/Persistence/ProjectSessionLoader.h>
#include <Ludus/Engine/Events/EventHandler.h>
#include <Ludus/Engine/Runtime/IHostContext.h>
#include <Ludus/Engine/Runtime/ISystem.h>

namespace Ludus::Editor::Commands
{
	struct ProjectSessionCommandContext;
	struct StartupCommandContext;
}

namespace Ludus::Editor::Core
{
	class EditorSystem final : public Ludus::Engine::Runtime::ISystem, public Ludus::Engine::Events::EventHandler
	{
	private:
		const Ludus::Editor::Persistence::EditorPersistence m_EditorPersistence;
		const Ludus::Editor::Core::EditorPersistenceContext m_Persistence;
		const Ludus::Editor::Persistence::ProjectSessionLoader m_ProjectSessionLoader;

		EditorConfiguration m_EditorConfiguration;
		EditorPreferences m_EditorPreferences;
		EditorStartupOptions m_EditorStartupOptions;

		EditorShell m_Shell;
		std::optional<ProjectSession> m_ProjectSession;
		Ludus::Engine::Runtime::IHostContext& m_HostContext;
		Ludus::Editor::Core::EditorSession m_Session;
		Ludus::Editor::Panels::PanelRegistry m_PanelRegistry;
		WelcomeWindow m_WelcomeWindow;

		Ludus::Editor::Commands::StartupCommandContext CreateStartupCommandContext();
		Ludus::Editor::Commands::ProjectSessionCommandContext CreateProjectSessionCommandContext();

		void DelegateUICommands();
		void DelegateEditCommands();
		void DelegateRequestCommands();

		void ApplyEditorPreferences();
		void ApplyStartupOptions();
		void RegisterPanels();

		void FlushCommands();
		void UpdateDialogs();
		void UpdatePanels();
		void UpdateShortcuts();
		void UpdateWindowTitle();

		void UpdateStartup();
		void UpdateProjectSession();

	public:
		EditorSystem(
			Ludus::Engine::Runtime::IHostContext& hostContext,
			EditorConfiguration editorConfiguration,
			Ludus::Editor::Persistence::EditorPersistence editorPersistence,
			EditorPreferences editorPreferences,
			EditorStartupOptions editorStartupOptions
		);
		~EditorSystem() = default;

		virtual void OnAttachImpl() override;
		virtual void OnDetachImpl() override;

		virtual void UpdateImpl(float deltaTime) override;

		virtual bool ProcessEvent(const Ludus::Engine::Events::Event& event) override;
	};
}
