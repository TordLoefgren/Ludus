#include "pch.h"

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/Requests/DeferredAction.h>
#include <Ludus/Editor/Commands/Requests/SceneActions.h>
#include <Ludus/Editor/Commands/Requests/Scenes.h>

namespace Ludus::Editor::Commands::Requests::Scenes
{
	void CreateScene(ProjectSessionCommandContext& context)
	{
		auto action = DeferredAction::CreateScene();
		if (TryOpenUnsavedChangesDialog(action, context))
		{
			return;
		}

		ExecuteDeferredAction(action, context);
	}

	void CreateSceneAs(const RequestCommand::CreateSceneAs& command, ProjectSessionCommandContext& context)
	{
		auto action = DeferredAction::CreateSceneAs(command.Path);
		if (TryOpenUnsavedChangesDialog(action, context))
		{
			return;
		}

		ExecuteDeferredAction(action, context);
	}

	void OpenScene(const RequestCommand::OpenScene& command, ProjectSessionCommandContext& context)
	{
		auto action = DeferredAction::OpenScene(command.Path);
		if (TryOpenUnsavedChangesDialog(action, context))
		{
			return;
		}

		ExecuteDeferredAction(action, context);
	}

	void SaveScene(const RequestCommand::SaveScene& command, ProjectSessionCommandContext& context)
	{
		SaveSceneAction(command.SceneId, context);
	}

	void SaveSceneAs(const RequestCommand::SaveSceneAs& command, ProjectSessionCommandContext& context)
	{
		SaveSceneAsAction(command.SceneId, command.Path, context);
	}

	void RenameScene(const RequestCommand::RenameScene& command, ProjectSessionCommandContext& context)
	{
		RenameSceneAction(command.SceneId, command.Path, context);
	}
}
