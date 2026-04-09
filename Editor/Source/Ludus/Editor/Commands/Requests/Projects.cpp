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
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Persistence/IRuntimeLaunchSettingsPersistence.h>
#include <Ludus/Engine/Persistence/IRuntimeManifestPersistence.h>
#include <Ludus/Engine/Persistence/IScenePersistence.h>
#include <Ludus/Engine/Runtime/RuntimeLaunchSettings.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::Editor::Commands::Requests::Projects
{
	namespace
	{
		bool HasUnresolvedScriptReferences(
			const Ludus::Engine::Core::Scene& scene,
			const Ludus::Engine::Runtime::RuntimeManifest& runtimeManifest
		)
		{
			for (const auto& script : scene.EntityComponentSystem.Scripts.View())
			{
				if (!runtimeManifest.TryGetScriptReference(script.Id))
				{
					return true;
				}
			}

			return false;
		}

		bool CanSaveActiveEditorScene(ProjectSessionCommandContext& context)
		{
			const auto activeSceneId = context.ProjectSession.EditorState.GetActiveSceneId();
			const auto& editorScene = context.ProjectSession.RuntimeState.GetEditorScene(activeSceneId);
			const auto& runtimeManifest = context.ProjectSession.Persistence.GetRuntimeManifest();
			if (HasUnresolvedScriptReferences(editorScene, runtimeManifest))
			{
				LUDUS_LOG_WARN("The project cannot be saved while the active editor scene contains unresolved script references.");
				return false;
			}

			return true;
		}

		bool CanTransitionProject(ProjectSessionCommandContext& context, std::string_view action)
		{
			if (!context.ProjectSession.EditorState.HasUnsavedChanges())
			{
				return true;
			}

			LUDUS_LOG_WARN(std::string(action) + " cannot be executed while the project has unsaved changes.");
			return false;
		}

		void CreateProject(
			const std::filesystem::path& projectRoot,
			std::string_view projectName,
			Ludus::Editor::Core::PendingProjectTransition& pendingProjectTransition,
			Ludus::Engine::Persistence::IScenePersistence& scenePersistence,
			Ludus::Engine::Persistence::IRuntimeManifestPersistence& runtimeManifestPersistence,
			Ludus::Engine::Persistence::IRuntimeLaunchSettingsPersistence& runtimeLaunchSettingsPersistence,
			Ludus::Editor::Persistence::IProjectManifestPersistence& projectManifestPersistence
		)
		{
			Ludus::Editor::Persistence::ProjectPaths::EnsureProjectLayoutExists(projectRoot);

			// Create default scene.
			const auto scene = Ludus::Editor::Core::ProjectTemplates::CreateDefaultScene();
			const auto scenePath = Ludus::Editor::Persistence::ProjectPaths::SceneFile(projectRoot, scene.Name);
			scenePersistence.Save(
				scene,
				scenePath
			);

			// Create runtime manifest.
			const auto runtimeManifest = Ludus::Engine::Runtime::RuntimeManifest::Create(
				scene.Id,
				{ { scene.Id, scene.Name, scenePath } },
				{ }
			);
			const auto runtimeManifestPath = Ludus::Engine::Persistence::Paths::RuntimeManifestFile(projectRoot, projectName);
			runtimeManifestPersistence.Save(
				runtimeManifest,
				runtimeManifestPath
			);

			// Create runtime settings.
			const auto runtimeLaunchSettings = Ludus::Engine::Runtime::RuntimeLaunchSettings();
			const auto runtimeLaunchSettingsPath = Ludus::Engine::Persistence::Paths::RuntimeLaunchSettingsFile(projectRoot, projectName);
			runtimeLaunchSettingsPersistence.Save(
				runtimeLaunchSettings,
				runtimeLaunchSettingsPath
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
			context.RuntimeLaunchSettingsPersistence,
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
			context.RuntimeLaunchSettingsPersistence,
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
		if (!CanTransitionProject(context, "Create project"))
		{
			return;
		}

		Ludus::Editor::Persistence::ProjectPaths::EnsureProjectsRootExists();
		const auto projectRoot = Ludus::Editor::Persistence::ProjectPaths::ProjectRoot(command.Name);
		CreateProject(
			projectRoot,
			command.Name,
			context.Shell.State.PendingProjectTransition,
			context.ScenePersistence,
			context.RuntimeManifestPersistence,
			context.RuntimeLaunchSettingsPersistence,
			context.ProjectManifestPersistence
		);

		Ludus::Editor::Panels::RefreshContentPanel(projectRoot, context.PanelRegistry);
	}

	void CreateProjectAs(const RequestCommand::CreateProjectAs& command, ProjectSessionCommandContext& context)
	{
		if (!CanTransitionProject(context, "Create project"))
		{
			return;
		}

		CreateProject(
			command.ProjectRoot,
			command.Name,
			context.Shell.State.PendingProjectTransition,
			context.ScenePersistence,
			context.RuntimeManifestPersistence,
			context.RuntimeLaunchSettingsPersistence,
			context.ProjectManifestPersistence
		);

		Ludus::Editor::Panels::RefreshContentPanel(command.ProjectRoot, context.PanelRegistry);
	}

	void OpenProject(const RequestCommand::OpenProject& command, ProjectSessionCommandContext& context)
	{
		if (!CanTransitionProject(context, "Open project"))
		{
			return;
		}

		auto projectManifest = context.ProjectManifestPersistence.Load(command.Path);
		context.Shell.State.PendingProjectTransition = Ludus::Editor::Core::PendingProjectTransition::OpenProject({ projectManifest });

		Ludus::Editor::Panels::RefreshContentPanel(projectManifest.ProjectRoot, context.PanelRegistry);
	}

	void SaveProject(ProjectSessionCommandContext& context)
	{
		auto& editorState = context.ProjectSession.EditorState;
		if (!editorState.HasUnsavedChanges())
		{
			return;
		}

		if (editorState.IsSceneDirty())
		{
			if (!editorState.ActiveSceneHasSavePath())
			{
				LUDUS_LOG_WARN("The project cannot be saved with the active scene missing a save path.");
				return;
			}

			if (!CanSaveActiveEditorScene(context))
			{
				return;
			}

			const auto activeSceneId = editorState.GetActiveSceneId();
			context.ScenePersistence.Save(
				context.ProjectSession.RuntimeState.GetEditorScene(activeSceneId),
				editorState.GetActiveSceneSavePath()
			);

			editorState.SetSceneDirty(false);
		}

		auto& persistence = context.ProjectSession.Persistence;

		if (editorState.IsProjectManifestDirty())
		{
			context.ProjectManifestPersistence.Save(
				persistence.GetProjectManifest(),
				persistence.GetProjectManifestPath()
			);

			editorState.SetProjectManifestDirty(false);
		}

		if (editorState.IsRuntimeManifestDirty())
		{
			context.RuntimeManifestPersistence.Save(
				persistence.GetRuntimeManifest(),
				persistence.GetRuntimeManifestPath()
			);

			editorState.SetRuntimeManifestDirty(false);
		}

		if (editorState.IsRuntimeLaunchSettingsDirty())
		{
			context.RuntimeLaunchSettingsPersistence.Save(
				persistence.GetRuntimeLaunchSettings(),
				persistence.GetRuntimeLaunchSettingsPath()
			);

			editorState.SetRuntimeLaunchSettingsDirty(false);
		}
	}

	void CloseProject(ProjectSessionCommandContext& context)
	{
		if (!CanTransitionProject(context, "Close project"))
		{
			return;
		}

		context.Shell.State.PendingProjectTransition = Ludus::Editor::Core::PendingProjectTransition::CloseProject();

		Ludus::Editor::Panels::ClearConsolePanel(context.PanelRegistry);
	}
}
