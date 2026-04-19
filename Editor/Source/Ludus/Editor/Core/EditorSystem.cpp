#include "pch.h"

#include <stdexcept>
#include <utility>
#include <vector>

#include <Ludus/Editor/Build/BuildManager.h>
#include <Ludus/Editor/Commands/EditCommand.h>
#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/StartupCommandContext.h>
#include <Ludus/Editor/Commands/UICommand.h>
#include <Ludus/Editor/Core/EditorConfiguration.h>
#include <Ludus/Editor/Core/EditorExecutionFlags.h>
#include <Ludus/Editor/Core/EditorPreferences.h>
#include <Ludus/Editor/Core/EditorShortcuts.h>
#include <Ludus/Editor/Core/EditorSystem.h>
#include <Ludus/Editor/Core/ProjectSessionContext.h>
#include <Ludus/Editor/Persistence/EditorPersistence.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Events/Event.h>
#include <Ludus/Engine/Events/EventType.h>
#include <Ludus/Engine/Persistence/EnginePersistence.h>

namespace Ludus::Editor::Core
{
	namespace
	{
		template<typename TCommand>
		void DelegateProjectSessionCommands(
			std::vector<TCommand>& stateCommands,
			Ludus::Editor::Commands::ProjectSessionCommandContext context
		)
		{
			std::vector<TCommand> commands;
			commands.swap(stateCommands);

			for (const auto& command : commands)
			{
				Ludus::Editor::Commands::Execute(command, context);
			}
		}

		template<typename TCommand>
		void DelegateStartupCommands(
			std::vector<TCommand>& stateCommands,
			Ludus::Editor::Commands::StartupCommandContext context
		)
		{
			std::vector<TCommand> commands;
			commands.swap(stateCommands);

			for (const auto& command : commands)
			{
				Ludus::Editor::Commands::Execute(command, context);
			}
		}
	}

	EditorSystem::EditorSystem(
		Ludus::Engine::Runtime::IHostContext& hostContext,
		EditorConfiguration editorConfiguration,
		Ludus::Editor::Persistence::EditorPersistence editorPersistence,
		EditorPreferences editorPreferences,
		EditorStartupOptions editorStartupOptions
	) :
		m_EditorPersistence(std::move(editorPersistence)),
		m_Persistence(
			m_EditorPersistence.EditorPreferences(),
			m_EditorPersistence.ProjectManifest(),
			hostContext.GetEnginePersistence().RuntimeLaunchSettings(),
			hostContext.GetEnginePersistence().RuntimeManifest(),
			hostContext.GetEnginePersistence().Scene()
		),
		m_ProjectSessionLoader(
			m_Persistence.Scene,
			m_Persistence.RuntimeManifest,
			m_Persistence.RuntimeLaunchSettings,
			m_Persistence.ProjectManifest
		),
		m_EditorConfiguration(std::move(editorConfiguration)),
		m_EditorPreferences(std::move(editorPreferences)),
		m_EditorStartupOptions(std::move(editorStartupOptions)),
		m_Shell(Ludus::Editor::Build::BuildManager::Create(
			m_Persistence.RuntimeManifest,
			m_Persistence.RuntimeLaunchSettings
		)),
		m_HostContext(hostContext),
		m_Session(
			m_ProjectSessionLoader,
			m_HostContext,
			m_Shell
		),
		m_PanelRegistry(),
		m_WelcomeWindow(m_EditorPreferences.RecentlyOpenedProjects)
	{}

	Ludus::Editor::Commands::StartupCommandContext EditorSystem::CreateStartupCommandContext()
	{
		return {
			.Shell = m_Shell,
			.HostContext = m_HostContext,
			.Preferences = m_EditorPreferences,
			.PanelRegistry = m_PanelRegistry,
			.Persistence = m_Persistence
		};
	}

	Ludus::Editor::Commands::ProjectSessionCommandContext EditorSystem::CreateProjectSessionCommandContext()
	{
		LUDUS_ASSERT(m_ProjectSession, "Project session context requires an active project.");

		return {
			.Shell = m_Shell,
			.HostContext = m_HostContext,
			.ProjectSession = *m_ProjectSession,
			.Preferences = m_EditorPreferences,
			.PanelRegistry = m_PanelRegistry,
			.Persistence = m_Persistence
		};
	}

	void EditorSystem::DelegateUICommands()
	{
		if (m_ProjectSession)
		{
			auto context = CreateProjectSessionCommandContext();

			DelegateProjectSessionCommands<Ludus::Editor::Commands::UICommand>(
				m_Shell.State.Commands.PendingCommands.UICommands,
				context
			);

			return;
		}

		auto context = CreateStartupCommandContext();

		DelegateStartupCommands<Ludus::Editor::Commands::UICommand>(
			m_Shell.State.Commands.PendingCommands.UICommands,
			context
		);
	}

	void EditorSystem::DelegateEditCommands()
	{
		if (m_ProjectSession)
		{
			auto context = CreateProjectSessionCommandContext();

			DelegateProjectSessionCommands<Ludus::Editor::Commands::EditCommand>(
				m_Shell.State.Commands.PendingCommands.EditCommands,
				context
			);

			return;
		}

		LUDUS_ASSERT(
			m_Shell.State.Commands.PendingCommands.EditCommands.empty(),
			"Edit commands are only valid while a project session is active."
		);
	}

	void EditorSystem::DelegateRequestCommands()
	{
		if (m_ProjectSession)
		{
			auto context = CreateProjectSessionCommandContext();

			DelegateProjectSessionCommands<Ludus::Editor::Commands::RequestCommand>(
				m_Shell.State.Commands.PendingCommands.RequestCommands,
				context
			);

			return;
		}

		auto context = CreateStartupCommandContext();

		DelegateStartupCommands<Ludus::Editor::Commands::RequestCommand>(
			m_Shell.State.Commands.PendingCommands.RequestCommands,
			context
		);
	}

	void EditorSystem::FlushCommands()
	{
		DelegateUICommands();
		DelegateEditCommands();
		DelegateRequestCommands();
	}

	void EditorSystem::UpdateDialogs()
	{
		if (auto commands = m_Shell.State.Dialogs.Update())
		{
			m_Shell.State.Commands.EnqueueCommands(std::move(*commands));
		}
	}

	void EditorSystem::UpdatePanels()
	{
		LUDUS_ASSERT(m_ProjectSession, "Panel updates require an active project session.");

		Ludus::Editor::Core::ProjectSessionContext context {
			.Shell = m_Shell,
			.ProjectSession = *m_ProjectSession,
			.HostContext = m_HostContext,
			.PanelRegistry = m_PanelRegistry
		};

		for (const auto& panel : m_PanelRegistry.View())
		{
			if (!panel->IsAvailable(context))
			{
				continue;
			}

			if (!panel->Update(context))
			{
				m_PanelRegistry.ScheduleRemove(panel->GetHandle());
			}
		}

		m_PanelRegistry.ApplyRemovals();
	}

	void EditorSystem::UpdateWindowTitle()
	{
		if (!m_ProjectSession)
		{
			m_HostContext.SetWindowTitle("Ludus Editor");
			return;
		}

		auto title = std::string("Ludus Editor - ") + m_ProjectSession->Persistence.GetProjectName();
		if (m_ProjectSession->EditorState.HasUnsavedChanges())
		{
			title += "*";
		}

		m_HostContext.SetWindowTitle(title);
	}

	void Ludus::Editor::Core::EditorSystem::ApplyEditorPreferences()
	{
		if (m_Shell.State.Theme.ActiveThemeId != m_EditorPreferences.ActiveThemeId)
		{
			m_Shell.State.Commands.AddRequestCommand(
				Ludus::Editor::Commands::RequestCommand::SetTheme { m_EditorPreferences.ActiveThemeId }
			);
		}
	}

	void Ludus::Editor::Core::EditorSystem::ApplyStartupOptions()
	{
		if (m_EditorStartupOptions.StartupProjectPath)
		{
			LUDUS_LOG_INFO("Using startup project from command line: " + m_EditorStartupOptions.StartupProjectPath->string());

			m_Shell.State.Commands.AddRequestCommand(
				Ludus::Editor::Commands::RequestCommand::OpenProject { *m_EditorStartupOptions.StartupProjectPath }
			);
		}

		if (m_EditorStartupOptions.EnableImGuiDemo)
		{
			LUDUS_LOG_INFO("ImGui demo panel enabled from command line.");
			m_Shell.State.Commands.AddRequestCommand(
				Ludus::Editor::Commands::RequestCommand::SetExecutionFlag { Ludus::Editor::Core::EditorExecutionFlags::ImGuiDemoEnabled }
			);
		}
	}

	void Ludus::Editor::Core::EditorSystem::RegisterPanels()
	{
		for (const auto& factoryMethod : m_EditorConfiguration.PanelFactories)
		{
			m_PanelRegistry.Register(factoryMethod());
		}
	}

	void Ludus::Editor::Core::EditorSystem::OnAttachImpl()
	{
		m_HostContext.SubscribeWindowCloseEvent(*this);

		ApplyEditorPreferences();
		ApplyStartupOptions();

		RegisterPanels();
	}

	void Ludus::Editor::Core::EditorSystem::OnDetachImpl()
	{
		m_HostContext.UnsubscribeWindowCloseEvent(*this);
		m_PanelRegistry.Clear();
	}

	bool EditorSystem::ProcessEvent(const Ludus::Engine::Events::Event& event)
	{
		if (event.Type != Ludus::Engine::Events::EventType::WindowCloseEvent)
		{
			return false;
		}

		m_Shell.State.Commands.AddRequestCommand(
			Ludus::Editor::Commands::RequestCommand::CloseApplication { }
		);

		return true;
	}

	void EditorSystem::UpdateShortcuts()
	{
		EditorShortcutContext shortcutContext {
			.Shell = m_Shell,
			.HostContext = m_HostContext,
			.PanelRegistry = m_PanelRegistry,
			.ProjectSession = m_ProjectSession ? &*m_ProjectSession : nullptr
		};

		Ludus::Editor::Core::DelegateShortcutCommands(shortcutContext);
	}

	void EditorSystem::UpdateStartup()
	{
		UpdateWindowTitle();

		if (auto commands = m_WelcomeWindow.Update())
		{
			m_Shell.State.Commands.EnqueueCommands(std::move(*commands));
		}

		FlushCommands();
		UpdateDialogs();
		FlushCommands();
	}

	void EditorSystem::UpdateProjectSession()
	{
		if (!m_ProjectSession)
		{
			throw std::runtime_error("No active project session available.");
		}

		// Resolve transition-based and shortcut-based commands.
		FlushCommands();
		UpdateWindowTitle();

		// Update panels and resolve commands.
		UpdatePanels();
		FlushCommands();
		UpdateWindowTitle();

		// Update dialogs and resolve commands.
		UpdateDialogs();
		FlushCommands();
		UpdateWindowTitle();

		m_Shell.State.Commands.ClearEntityReferences();
	}

	void Ludus::Editor::Core::EditorSystem::UpdateImpl(float deltaTime)
	{
		(void)deltaTime;

		m_Session.ApplyTransitions(m_Shell.State.PendingProjectTransition, m_ProjectSession);

		UpdateShortcuts();

		if (m_Shell.State.Mode == Ludus::Editor::Core::EditorMode::Startup)
		{
			UpdateStartup();
			return;
		}

		UpdateProjectSession();
	}
}
