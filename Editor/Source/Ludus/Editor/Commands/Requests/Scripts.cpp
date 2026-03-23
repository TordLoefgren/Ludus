#include "pch.h"

#include <filesystem>
#include <stdexcept>

#include <Ludus/Editor/Commands/EditCommand.h>
#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/Requests/Scripts.h>
#include <Ludus/Editor/Panels/PanelHelpers.h>
#include <Ludus/Editor/Persistence/Paths.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Editor::Commands::Requests::Scripts
{
	void CreateScript(const RequestCommand::CreateScript& command, ProjectSessionCommandContext& context)
	{
		const auto projectRoot = context.ProjectSession.GetProjectRoot();
		const auto scriptSourcePath = Ludus::Editor::Persistence::Paths::ScriptSourceFile(projectRoot, command.Name);

		if (context.ProjectSession.HasPersistenceScriptReference(command.Name))
		{
			throw std::runtime_error("Script already exists in the runtime manifest: " + command.Name);
		}

		if (std::filesystem::exists(scriptSourcePath))
		{
			throw std::runtime_error("Script source file already exists: " + scriptSourcePath.string());
		}

		const auto handle = context.ProjectSession.AllocatePersistenceScriptHandle();
		auto& runtimeManifest = context.ProjectSession.GetPersistenceRuntimeManifest();
		context.ProjectSession.AddOrUpdatePersistenceScriptReference(handle, command.Name);
		context.RuntimeManifestPersistence.Save(runtimeManifest, context.ProjectSession.ProjectManifest.RuntimeManifestPath);

		auto& build = context.Shell.Build;

		try
		{
			build.CreateScript(projectRoot, command.Name);
		}
		catch (...)
		{
			// Make sure to roll back persistence in case of exceptions.
			context.ProjectSession.RemovePersistenceScriptReference(handle);
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

	void BuildScript(const RequestCommand::BuildScript& command, ProjectSessionCommandContext& context)
	{
		LUDUS_ASSERT(!context.ProjectSession.IsSimulating(), "Scripts cannot be built while the simulation session is active.");

		const auto projectRoot = context.ProjectSession.GetProjectRoot();
		auto& build = context.Shell.Build;

		switch (command.BuildCommand)
		{
			case Ludus::Editor::Build::BuildCommand::Build:
				build.Build(projectRoot);
				break;
			case Ludus::Editor::Build::BuildCommand::Rebuild:
				build.Rebuild(projectRoot);
				break;
			case Ludus::Editor::Build::BuildCommand::Clean:
				build.Clean(projectRoot);
				break;
		}

		Ludus::Editor::Panels::RefreshContentPanel(projectRoot, context.PanelRegistry);
	}
}
