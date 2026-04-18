#include "pch.h"

#include <filesystem>

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/Requests/DeferredAction.h>
#include <Ludus/Editor/Commands/Requests/SceneActions.h>
#include <Ludus/Editor/Commands/Requests/Scenes.h>
#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/Engine/Platform/Modals.h>

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
		const auto scenePath = context.ProjectSession.Persistence.TryGetScenePath(command.SceneId);
		if (!scenePath)
		{
			// Unsaved scenes need a first-time Save As path before they can be saved to a path implicitly.
			std::filesystem::path path;
			if (Ludus::Engine::Platform::Modals::SaveFileDialog(
				path,
				Ludus::Engine::Persistence::Paths::Constants::SceneExtension,
				Ludus::Engine::Persistence::Paths::ScenesDirectory(context.ProjectSession.Persistence.GetProjectRoot()),
				"Untitled"
			))
			{
				context.Shell.State.Commands.AddRequestCommand(
					Ludus::Editor::Commands::RequestCommand::SaveSceneAs { .SceneId = command.SceneId, .Path = path }
				);
			}

			return;
		}

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
