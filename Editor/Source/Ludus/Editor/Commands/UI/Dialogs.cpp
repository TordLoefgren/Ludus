#include "pch.h"

#include <unordered_map>
#include <utility>

#include <Ludus/Editor/Commands/CommandContext.h>
#include <Ludus/Editor/Commands/UI/Dialogs.h>
#include <Ludus/Editor/Core/EditorContext.h>
#include <Ludus/Editor/Dialogs/CreateProjectDialog.h>
#include <Ludus/Editor/Dialogs/DialogManager.h>
#include <Ludus/Engine/Components/ScriptComponent.h>
#include <Ludus/Engine/Core/SystemContext.h>

namespace Ludus::Editor::Commands::UI::Dialogs
{
	void OpenAddScriptDialog(const UICommand::OpenAddScriptDialog& command, CommandContext& context)
	{
		if (!context.SystemContext.HasProjectContext())
		{
			LUDUS_LOG_ERROR("Script cannot be added. No project context found.");
			return;
		}

		auto& projectContext = context.SystemContext.ProjectContext.value();
		auto scriptHandlesByName = std::unordered_map<std::string, Ludus::Engine::Components::ScriptHandle> { };
		for (const auto& reference : projectContext.Project.Scripts)
		{
			scriptHandlesByName.emplace(reference.Name, reference.Handle);
		}

		Ludus::Editor::Dialogs::AddScriptDialog dialog(
			command.Entity,
			command.Scene,
			projectContext.GetScriptNames(),
			std::move(scriptHandlesByName)
		);
		context.EditorContext.State.Dialogs.Open(dialog);
	}

	void OpenCreateProjectDialog(const UICommand::OpenCreateProjectDialog& command, CommandContext& context)
	{
		(void)command;
		context.EditorContext.State.Dialogs.Open<Ludus::Editor::Dialogs::CreateProjectDialog>();
	}
}
