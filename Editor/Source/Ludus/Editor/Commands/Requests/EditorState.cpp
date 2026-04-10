#include "pch.h"

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/Requests/DeferredAction.h>
#include <Ludus/Editor/Commands/Requests/EditorState.h>
#include <Ludus/Editor/Commands/Requests/EditorStateActions.h>
#include <Ludus/Editor/Commands/Requests/Projects.h>
#include <Ludus/Editor/Commands/StartupCommandContext.h>
#include <Ludus/Editor/Dialogs/UnsavedChangesResult.h>

namespace Ludus::Editor::Commands::Requests::EditorState
{
	void CloseApplication(ProjectSessionCommandContext& context)
	{
		auto action = DeferredAction::CloseApplication();
		if (TryOpenUnsavedChangesDialog(action, context))
		{
			return;
		}

		ExecuteDeferredAction(action, context);
	}

	void ResolveUnsavedChanges(const RequestCommand::ResolveUnsavedChanges& command, ProjectSessionCommandContext& context)
	{
		if (command.Result == Ludus::Editor::Dialogs::UnsavedChangesResult::DontSave)
		{
			ExecuteDeferredAction(command.DeferredAction, context);
			return;
		}

		if (command.Result == Ludus::Editor::Dialogs::UnsavedChangesResult::Save)
		{
			Ludus::Editor::Commands::Requests::Projects::SaveProject(context);

			if (context.ProjectSession.EditorState.HasUnsavedChanges())
			{
				LUDUS_LOG_WARN("Deferred action was not resumed because saving did not clear all unsaved changes.");
				return;
			}

			ExecuteDeferredAction(command.DeferredAction, context);
			return;
		}

		if (command.Result == Ludus::Editor::Dialogs::UnsavedChangesResult::Cancel)
		{
			return;
		}
	}

	void SetExecutionFlag(const RequestCommand::SetExecutionFlag& command, ProjectSessionCommandContext& context)
	{
		SetExecutionFlagAction(command.Flag, context);
	}

	void SetExecutionMode(const RequestCommand::SetExecutionMode& command, ProjectSessionCommandContext& context)
	{
		SetExecutionModeAction(command.Mode, context);
	}

	void SetTheme(const RequestCommand::SetTheme& command, ProjectSessionCommandContext& context)
	{
		SetThemeAction(command.ThemeId, context);
	}

	void UnsetExecutionFlag(const RequestCommand::UnsetExecutionFlag& command, ProjectSessionCommandContext& context)
	{
		UnsetExecutionFlagAction(command.Flag, context);
	}

	void CloseApplication(StartupCommandContext& context)
	{
		CloseApplicationAction(context);
	}

	void SetExecutionFlag(const RequestCommand::SetExecutionFlag& command, StartupCommandContext& context)
	{
		SetExecutionFlagAction(command.Flag, context);
	}

	void UnsetExecutionFlag(const RequestCommand::UnsetExecutionFlag& command, StartupCommandContext& context)
	{
		UnsetExecutionFlagAction(command.Flag, context);
	}
}
