#include "pch.h"

#include <filesystem>
#include <stdexcept>

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/Requests/Scripts.h>
#include <Ludus/Editor/Panels/PanelHelpers.h>
#include <Ludus/Editor/Persistence/ProjectPaths.h>

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

		const auto handle = context.ProjectSession.AllocateEditorScriptHandle();
		auto& runtimeManifest = context.ProjectSession.GetEditorManifest();
		context.ProjectSession.AddOrUpdateEditorScriptReference(handle, command.Name);
		context.RuntimeManifestPersistence.Save(runtimeManifest, context.ProjectSession.ProjectManifest.RuntimeManifestPath);

		auto& build = context.Shell.Build;

		try
		{
			build.CreateScript(projectRoot, command.Name);
		}
		catch (...)
		{
			// Make sure to roll back persistence in case of exceptions.
			context.ProjectSession.RemoveEditorScriptReference(handle);
			context.RuntimeManifestPersistence.Save(runtimeManifest, context.ProjectSession.ProjectManifest.RuntimeManifestPath);
			Ludus::Editor::Panels::RefreshContentPanel(projectRoot, context.PanelRegistry);

			throw;
		}

		context.Shell.State.Commands.AddEditCommand(
			Ludus::Editor::Commands::EditCommand::AddComponent<Ludus::Engine::Components::ScriptComponent> {
			.EntityReference = command.EntityReference,
				.SceneHandle = command.SceneHandle,
				.Init = Ludus::Engine::Components::ScriptComponent { command.Name, handle }
		});

		Ludus::Editor::Panels::RefreshContentPanel(projectRoot, context.PanelRegistry);
	}
}
