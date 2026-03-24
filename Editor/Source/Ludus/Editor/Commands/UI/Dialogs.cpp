#include "pch.h"

#include <filesystem>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/StartupCommandContext.h>
#include <Ludus/Editor/Commands/UI/Dialogs.h>
#include <Ludus/Editor/Dialogs/CreateProjectDialog.h>
#include <Ludus/Editor/Dialogs/DialogManager.h>
#include <Ludus/Engine/Components/ScriptComponent.h>
#include <Ludus/Engine/Persistence/Paths.h>

namespace Ludus::Editor::Commands::UI::Dialogs
{
	void OpenAddScriptDialog(const UICommand::OpenAddScriptDialog& command, ProjectSessionCommandContext& context)
	{
		auto scriptHandlesByName = std::unordered_map<std::string, Ludus::Engine::Components::ScriptHandle> { };
		for (const auto& reference : context.ProjectSession.GetEditorManifest().Scripts)
		{
			scriptHandlesByName.emplace(reference.Name, reference.Handle);
		}

		Ludus::Editor::Dialogs::AddScriptDialog dialog(
			command.EntityHandle,
			command.SceneHandle,
			context.ProjectSession.GetEditorScriptNames(),
			std::move(scriptHandlesByName)
		);
		context.Shell.State.Dialogs.Open(dialog);
	}

	void OpenCreateProjectDialog(ProjectSessionCommandContext& context)
	{
		context.Shell.State.Dialogs.Open<Ludus::Editor::Dialogs::CreateProjectDialog>();
	}

	void OpenCreateProjectDialog(StartupCommandContext& context)
	{
		context.Shell.State.Dialogs.Open<Ludus::Editor::Dialogs::CreateProjectDialog>();
	}
}
