#include "pch.h"

#include <filesystem>
#include <stdexcept>

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/Requests/Scripts.h>
#include <Ludus/Editor/Panels/PanelHelpers.h>
#include <Ludus/Editor/Persistence/ProjectPaths.h>
#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Editor::Commands::Requests::Scripts
{
	void CreateScript(const RequestCommand::CreateScript& command, ProjectSessionCommandContext& context)
	{
		const auto projectRoot = context.ProjectSession.Persistence.GetProjectRoot();
		const auto scriptSourcePath = Ludus::Editor::Persistence::ProjectPaths::ScriptSourceFile(projectRoot, command.Name);

		if (context.ProjectSession.Persistence.HasScriptReference(command.Name))
		{
			throw std::runtime_error("Script already exists in the runtime manifest: " + command.Name);
		}

		if (std::filesystem::exists(scriptSourcePath))
		{
			throw std::runtime_error("Script source file already exists: " + scriptSourcePath.string());
		}

		const auto wasRuntimeManifestDirty = context.ProjectSession.EditorState.IsRuntimeManifestDirty();
		const auto id = context.ProjectSession.Persistence.AllocateScriptId();
		context.ProjectSession.AddOrUpdateScriptReference(id, command.Name);

		auto& build = context.Shell.Build;

		try
		{
			build.CreateScript(projectRoot, command.Name);
		}
		catch (...)
		{
			// Make sure to roll back live project state in case of exceptions.
			context.ProjectSession.RemoveScriptReference(id);
			if (!wasRuntimeManifestDirty)
			{
				context.ProjectSession.EditorState.SetRuntimeManifestDirty(false);
			}
			Ludus::Editor::Panels::RefreshContentPanel(projectRoot, context.PanelRegistry);

			throw;
		}

		context.Shell.State.Commands.AddEditCommand(
			Ludus::Editor::Commands::EditCommand::AddComponent<Ludus::Engine::Components::ScriptComponent> {
			.SceneId = command.SceneId, .EntityReference = command.EntityReference, .Init = Ludus::Engine::Components::ScriptComponent { id }
		});

		Ludus::Editor::Panels::RefreshContentPanel(projectRoot, context.PanelRegistry);
	}
}
