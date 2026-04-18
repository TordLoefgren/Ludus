#include "pch.h"

#include <variant>

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/Requests/BuildActions.h>
#include <Ludus/Editor/Commands/Requests/DeferredAction.h>
#include <Ludus/Editor/Commands/Requests/EditorStateActions.h>
#include <Ludus/Editor/Commands/Requests/ProjectActions.h>
#include <Ludus/Editor/Commands/Requests/ProjectTransitionContext.h>
#include <Ludus/Editor/Commands/Requests/SceneActions.h>
#include <Ludus/Editor/Commands/UICommand.h>

namespace Ludus::Editor::Commands::Requests
{
	namespace
	{
		Projects::ProjectTransitionContext MakeProjectTransitionContext(ProjectSessionCommandContext& context)
		{
			return {
				.Shell = context.Shell,
				.Preferences = context.Preferences,
				.PanelRegistry = context.PanelRegistry,
				.Persistence = context.Persistence
			};
		}
	}

	void ExecuteDeferredAction(
		const DeferredAction& deferredAction,
		ProjectSessionCommandContext& context
	)
	{
		using DeferredAction = DeferredAction;

		std::visit([&](auto&& value)
		{
			using Alt = std::decay_t<decltype(value)>;

			if constexpr (std::is_same_v<Alt, DeferredAction::BuildRuntimeDeferredAction>)
			{
				Builds::BuildRuntimeAction(value.BuildConfiguration, context);
			}
			else if constexpr (std::is_same_v<Alt, DeferredAction::CloseApplicationDeferredAction>)
			{
				EditorState::CloseApplicationAction(context);
			}
			else if constexpr (std::is_same_v<Alt, DeferredAction::CloseProjectDeferredAction>)
			{
				Projects::CloseProjectAction(context);
			}
			else if constexpr (std::is_same_v<Alt, DeferredAction::CreateProjectDeferredAction>)
			{
				Projects::CreateProjectAction(value.Name, MakeProjectTransitionContext(context));
			}
			else if constexpr (std::is_same_v<Alt, DeferredAction::CreateProjectAsDeferredAction>)
			{
				Projects::CreateProjectAsAction(value.Path, value.Name, MakeProjectTransitionContext(context));
			}
			else if constexpr (std::is_same_v<Alt, DeferredAction::CreateSceneDeferredAction>)
			{
				Scenes::CreateSceneAction(context);
			}
			else if constexpr (std::is_same_v<Alt, DeferredAction::CreateSceneAsDeferredAction>)
			{
				Scenes::CreateSceneAsAction(value.Path, context);
			}
			else if constexpr (std::is_same_v<Alt, DeferredAction::OpenProjectDeferredAction>)
			{
				Projects::OpenProjectAction(value.Path, MakeProjectTransitionContext(context));
			}
			else if constexpr (std::is_same_v<Alt, DeferredAction::OpenSceneDeferredAction>)
			{
				Scenes::OpenSceneAction(value.Path, context);
			}
			else
			{
				throw std::runtime_error("Unsupported unsaved changes deferred action.");
			}
		}, deferredAction.Data);
	}

	bool TryOpenUnsavedChangesDialog(
		const DeferredAction& action,
		ProjectSessionCommandContext& context
	)
	{
		if (context.ProjectSession.EditorState.HasUnsavedChanges())
		{
			context.Shell.State.Commands.AddUICommand(
				UICommand::OpenUnsavedChangesDialog { .DeferredAction = action }
			);

			return true;
		}

		return false;
	}
}
