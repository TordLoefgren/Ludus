#include "pch.h"

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/Requests/DeferredAction.h>
#include <Ludus/Editor/Commands/Requests/ProjectActions.h>
#include <Ludus/Editor/Commands/Requests/ProjectTransitionContext.h>
#include <Ludus/Editor/Commands/Requests/Projects.h>
#include <Ludus/Editor/Commands/StartupCommandContext.h>

namespace Ludus::Editor::Commands::Requests::Projects
{
	namespace
	{
		ProjectTransitionContext MakeProjectTransitionContext(StartupCommandContext& context)
		{
			return {
				.Shell = context.Shell,
				.Preferences = context.Preferences,
				.PanelRegistry = context.PanelRegistry,
				.Persistence = context.Persistence
			};
		}
	}

	void CreateProject(const RequestCommand::CreateProject& command, StartupCommandContext& context)
	{
		CreateProjectAction(command.Name, MakeProjectTransitionContext(context));
	}

	void CreateProjectAs(const RequestCommand::CreateProjectAs& command, StartupCommandContext& context)
	{
		CreateProjectAsAction(command.Path, command.Name, MakeProjectTransitionContext(context));
	}

	void OpenProject(const RequestCommand::OpenProject& command, StartupCommandContext& context)
	{
		OpenProjectAction(command.Path, MakeProjectTransitionContext(context));
	}

	void CreateProject(const RequestCommand::CreateProject& command, ProjectSessionCommandContext& context)
	{
		auto action = DeferredAction::CreateProject(command.Name);
		if (TryOpenUnsavedChangesDialog(action, context))
		{
			return;
		}

		ExecuteDeferredAction(action, context);
	}

	void CreateProjectAs(const RequestCommand::CreateProjectAs& command, ProjectSessionCommandContext& context)
	{
		auto action = DeferredAction::CreateProjectAs(command.Name, command.Path);
		if (TryOpenUnsavedChangesDialog(action, context))
		{
			return;
		}

		ExecuteDeferredAction(action, context);
	}

	void OpenProject(const RequestCommand::OpenProject& command, ProjectSessionCommandContext& context)
	{
		auto action = DeferredAction::OpenProject(command.Path);
		if (TryOpenUnsavedChangesDialog(action, context))
		{
			return;
		}

		ExecuteDeferredAction(action, context);
	}

	void SaveProject(ProjectSessionCommandContext& context)
	{
		SaveProjectAction(context);
	}

	void CloseProject(ProjectSessionCommandContext& context)
	{
		auto action = DeferredAction::CloseProject();
		if (TryOpenUnsavedChangesDialog(action, context))
		{
			return;
		}

		ExecuteDeferredAction(action, context);
	}
}
