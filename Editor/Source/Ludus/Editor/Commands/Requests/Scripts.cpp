#include "pch.h"

#include <utility>

#include <Ludus/Editor/Commands/EditCommand.h>
#include <Ludus/Editor/Commands/Requests/Projects.h>
#include <Ludus/Editor/Core/EditorContext.h>
#include <Ludus/Engine/Core/SystemContext.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Persistence/ProjectRepository.h>

namespace Ludus::Editor::Commands::Requests::Scripts
{
	void CreateScript(const RequestCommand::CreateScript& command, CommandContext& context)
	{
		auto& systemContext = context.SystemContext;
		if (!systemContext.HasProjectContext())
		{
			LUDUS_LOG_WARN("Cannot create script. No project context exists.");
			return;
		}

		auto& projectContext = systemContext.ProjectContext.value();

		auto& build = context.EditorContext.Build;
		build.EnsureScriptProject(projectContext);
		build.CreateScript(projectContext, command.Name);

		// Persist script references to project immediately.
		systemContext.ProjectRepository.SaveProject(projectContext);
		context.EditorContext.Session.MarkProjectDirty(false);

		const auto handle = projectContext.TryFindScriptHandleByName(command.Name);
		if (!handle.has_value())
		{
			LUDUS_LOG_WARN("Failed to resolve script handle for script: " + command.Name);
			return;
		}

		context.EditorContext.State.Commands.AddEditCommand(
			Ludus::Editor::Commands::EditCommand::AddComponent<Ludus::Engine::Components::ScriptComponent> {
			.Entity = command.Entity, .Scene = command.Scene, .Init = Ludus::Engine::Components::ScriptComponent { command.Name, handle.value() }
		});
	}
}
