#include "pch.h"

#include <filesystem>

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/Requests/Projects.h>
#include <Ludus/Editor/Commands/StartupCommandContext.h>
#include <Ludus/Editor/Core/PendingProjectTransition.h>
#include <Ludus/Editor/Core/ProjectTemplates.h>
#include <Ludus/Editor/Panels/PanelHelpers.h>
#include <Ludus/Editor/Persistence/IProjectManifestPersistence.h>
#include <Ludus/Editor/Persistence/ProjectPaths.h>
#include <Ludus/Engine/Persistence/IRuntimeManifestPersistence.h>
#include <Ludus/Engine/Persistence/IScenePersistence.h>

namespace Ludus::Editor::Commands::Requests::Projects
{
	namespace
	{
		void CreateProject(
			const std::filesystem::path& projectRoot,
			std::string_view projectName,
			Ludus::Editor::Core::PendingProjectTransition& pendingProjectTransition,
			Ludus::Engine::Persistence::IScenePersistence& scenePersistence,
			Ludus::Engine::Persistence::IRuntimeManifestPersistence& runtimeManifestPersistence,
			Ludus::Editor::Persistence::IProjectManifestPersistence& projectManifestPersistence
		)
		{
			Ludus::Editor::Persistence::ProjectPaths::EnsureProjectLayoutExists(projectRoot);

			// Create default scene.
			const auto scene = Ludus::Editor::Core::ProjectTemplates::CreateDefaultScene();
			const auto scenePath = Ludus::Engine::Persistence::Paths::SceneFile(projectRoot, scene.Name);
			scenePersistence.Save(
				scene,
				scenePath
			);

			// Create runtime manifest.
			const auto runtimeManifest = Ludus::Engine::Runtime::RuntimeManifest::Create(
				scene.Handle,
				{ { scene.Handle, scene.Name, scenePath } },
				{ }
			);
			const auto runtimeManifestPath = Ludus::Engine::Persistence::Paths::RuntimeManifestFile(projectRoot, projectName);
			runtimeManifestPersistence.Save(
				runtimeManifest,
				runtimeManifestPath
			);

			// Create project manifest.
			const auto projectManifest = Ludus::Editor::Core::ProjectManifest::Create(
				projectRoot,
				runtimeManifestPath
			);
			projectManifestPersistence.Save(
				projectManifest,
				Ludus::Editor::Persistence::ProjectPaths::ProjectManifestFile(projectRoot, projectName)
			);

			// Set pending project.
			pendingProjectTransition = Ludus::Editor::Core::PendingProjectTransition::OpenProject({ projectManifest });
		}
	}

	void CreateProject(const RequestCommand::CreateProject& command, StartupCommandContext& context)
	{
		Ludus::Editor::Persistence::ProjectPaths::EnsureProjectsRootExists();
		const auto projectRoot = Ludus::Editor::Persistence::ProjectPaths::ProjectRoot(command.Name);
		CreateProject(
			projectRoot,
			command.Name,
			context.Shell.State.PendingProjectTransition,
			context.ScenePersistence,
			context.RuntimeManifestPersistence,
			context.ProjectManifestPersistence
		);

		Ludus::Editor::Panels::RefreshContentPanel(projectRoot, context.PanelRegistry);
	}

	void CreateProjectAs(const RequestCommand::CreateProjectAs& command, StartupCommandContext& context)
	{
		CreateProject(
			command.ProjectRoot,
			command.Name,
			context.Shell.State.PendingProjectTransition,
			context.ScenePersistence,
			context.RuntimeManifestPersistence,
			context.ProjectManifestPersistence
		);

		Ludus::Editor::Panels::RefreshContentPanel(command.ProjectRoot, context.PanelRegistry);
	}

	void OpenProject(const RequestCommand::OpenProject& command, StartupCommandContext& context)
	{
		auto projectManifest = context.ProjectManifestPersistence.Load(command.Path);
		context.Shell.State.PendingProjectTransition = Ludus::Editor::Core::PendingProjectTransition::OpenProject({ projectManifest });

		Ludus::Editor::Panels::RefreshContentPanel(projectManifest.ProjectRoot, context.PanelRegistry);
	}

	void CreateProject(const RequestCommand::CreateProject& command, ProjectSessionCommandContext& context)
	{
		Ludus::Editor::Persistence::ProjectPaths::EnsureProjectsRootExists();
		const auto projectRoot = Ludus::Editor::Persistence::ProjectPaths::ProjectRoot(command.Name);
		CreateProject(
			projectRoot,
			command.Name,
			context.Shell.State.PendingProjectTransition,
			context.ScenePersistence,
			context.RuntimeManifestPersistence,
			context.ProjectManifestPersistence
		);

		Ludus::Editor::Panels::RefreshContentPanel(projectRoot, context.PanelRegistry);
	}

	void CreateProjectAs(const RequestCommand::CreateProjectAs& command, ProjectSessionCommandContext& context)
	{
		CreateProject(
			command.ProjectRoot,
			command.Name,
			context.Shell.State.PendingProjectTransition,
			context.ScenePersistence,
			context.RuntimeManifestPersistence,
			context.ProjectManifestPersistence
		);

		Ludus::Editor::Panels::RefreshContentPanel(command.ProjectRoot, context.PanelRegistry);
	}

	void OpenProject(const RequestCommand::OpenProject& command, ProjectSessionCommandContext& context)
	{
		auto projectManifest = context.ProjectManifestPersistence.Load(command.Path);
		context.Shell.State.PendingProjectTransition = Ludus::Editor::Core::PendingProjectTransition::OpenProject({ projectManifest });

		Ludus::Editor::Panels::RefreshContentPanel(projectManifest.ProjectRoot, context.PanelRegistry);
	}

	void CloseProject(ProjectSessionCommandContext& context)
	{
		context.Shell.State.PendingProjectTransition = Ludus::Editor::Core::PendingProjectTransition::CloseProject();

		Ludus::Editor::Panels::ClearConsolePanel(context.PanelRegistry);
	}
}
