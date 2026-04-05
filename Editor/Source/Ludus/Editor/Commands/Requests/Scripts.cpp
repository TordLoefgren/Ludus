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
		const auto projectRoot = context.ProjectSession.GetProjectRoot();
		const auto scriptSourcePath = Ludus::Editor::Persistence::ProjectPaths::ScriptSourceFile(projectRoot, command.Name);

		if (context.ProjectSession.HasEditorScriptReference(command.Name))
		{
			throw std::runtime_error("Script already exists in the runtime manifest: " + command.Name);
		}

		if (std::filesystem::exists(scriptSourcePath))
		{
			throw std::runtime_error("Script source file already exists: " + scriptSourcePath.string());
		}

		const auto id = context.ProjectSession.AllocateEditorScriptId();
		auto& runtimeManifest = context.ProjectSession.GetEditorManifest();
		context.ProjectSession.AddOrUpdateEditorScriptReference(id, command.Name);
		context.RuntimeManifestPersistence.Save(runtimeManifest, context.ProjectSession.ProjectManifest.RuntimeManifestPath);

		auto& build = context.Shell.Build;

		try
		{
			build.CreateScript(projectRoot, command.Name);
		}
		catch (...)
		{
			// Make sure to roll back persistence in case of exceptions.
			context.ProjectSession.RemoveEditorScriptReference(id);
			context.RuntimeManifestPersistence.Save(runtimeManifest, context.ProjectSession.ProjectManifest.RuntimeManifestPath);
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
