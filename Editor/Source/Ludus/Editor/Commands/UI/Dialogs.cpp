#include "pch.h"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <utility>

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/StartupCommandContext.h>
#include <Ludus/Editor/Commands/UI/Dialogs.h>
#include <Ludus/Editor/Dialogs/CreateProjectDialog.h>
#include <Ludus/Editor/Dialogs/DialogManager.h>
#include <Ludus/Editor/Dialogs/RenameSceneDialog.h>
#include <Ludus/Editor/Dialogs/UnsavedChangesDialog.h>
#include <Ludus/Engine/Components/ScriptComponent.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/Engine/Platform/Modals.h>

namespace Ludus::Editor::Commands::UI::Dialogs
{
	void OpenAddScriptDialog(const UICommand::OpenAddScriptDialog& command, ProjectSessionCommandContext& context)
	{
		Ludus::Editor::Dialogs::AddScriptDialog dialog(
			command.SceneId,
			command.EntityId,
			context.ProjectSession.Persistence.GetScriptNames(),
			context.ProjectSession.Persistence.GetScripts()
		);
		context.Shell.State.Dialogs.Open(dialog);
	}

	void OpenCreateProjectDialog(ProjectSessionCommandContext& context)
	{
		context.Shell.State.Dialogs.Open<Ludus::Editor::Dialogs::CreateProjectDialog>();
	}

	void OpenRenameSceneDialog(const UICommand::OpenRenameSceneDialog& command, ProjectSessionCommandContext& context)
	{
		const auto scenePath = context.ProjectSession.Persistence.TryGetScenePath(command.SceneId);
		if (!scenePath)
		{
			// Unsaved scenes need a first-time Save As path before they can be renamed.
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

		Ludus::Editor::Dialogs::RenameSceneDialog dialog(
			command.SceneId,
			*scenePath
		);
		context.Shell.State.Dialogs.Open(dialog);
	}

	void OpenUnsavedChangesDialog(const UICommand::OpenUnsavedChangesDialog& command, ProjectSessionCommandContext& context)
	{
		Ludus::Editor::Dialogs::UnsavedChangesDialog dialog(
			command.DeferredAction
		);
		context.Shell.State.Dialogs.Open(dialog);
	}

	void OpenCreateProjectDialog(StartupCommandContext& context)
	{
		context.Shell.State.Dialogs.Open<Ludus::Editor::Dialogs::CreateProjectDialog>();
	}
}
