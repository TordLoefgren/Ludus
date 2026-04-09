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
#include <Ludus/Engine/Components/ScriptComponent.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Persistence/Paths.h>

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
			throw std::runtime_error("Scene does not have a path.");
		}

		Ludus::Editor::Dialogs::RenameSceneDialog dialog(
			command.SceneId,
			*scenePath
		);
		context.Shell.State.Dialogs.Open(dialog);
	}

	void OpenCreateProjectDialog(StartupCommandContext& context)
	{
		context.Shell.State.Dialogs.Open<Ludus::Editor::Dialogs::CreateProjectDialog>();
	}
}
