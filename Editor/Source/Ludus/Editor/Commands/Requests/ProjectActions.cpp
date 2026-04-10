#include "pch.h"

#include <filesystem>
#include <string>
#include <string_view>

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/Requests/ProjectTransitionContext.h>
#include <Ludus/Editor/Core/ProjectTemplates.h>
#include <Ludus/Editor/Core/SceneQueries.h>
#include <Ludus/Editor/Panels/PanelHelpers.h>
#include <Ludus/Editor/Persistence/ProjectPaths.h>

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

		void RefreshContentPanel(
			const std::filesystem::path& projectRoot,
			ProjectTransitionContext context
		)
		{
			Ludus::Editor::Panels::RefreshContentPanel(projectRoot, context.PanelRegistry);
		}
	}

	void CreateProjectAction(const std::string& name, ProjectTransitionContext context)
	{
		Ludus::Editor::Persistence::ProjectPaths::EnsureProjectsRootExists();

		const auto projectRoot = Ludus::Editor::Persistence::ProjectPaths::ProjectRoot(name);
		CreateProject(
			projectRoot,
			name,
			context.Shell.State.PendingProjectTransition,
			context.ScenePersistence,
			context.RuntimeManifestPersistence,
			context.RuntimeLaunchSettingsPersistence,
			context.ProjectManifestPersistence
		);

		RefreshContentPanel(projectRoot, context);
	}

	void CreateProjectAsAction(const std::filesystem::path& path, const std::string& name, ProjectTransitionContext context)
	{
		Ludus::Editor::Persistence::ProjectPaths::EnsureProjectsRootExists();

		CreateProject(
			path,
			name,
			context.Shell.State.PendingProjectTransition,
			context.ScenePersistence,
			context.RuntimeManifestPersistence,
			context.RuntimeLaunchSettingsPersistence,
			context.ProjectManifestPersistence
		);

		RefreshContentPanel(path, context);
	}

	void OpenProjectAction(const std::filesystem::path& path, ProjectTransitionContext context)
	{
		auto projectManifest = context.ProjectManifestPersistence.Load(path);
		const auto projectRoot = projectManifest.ProjectRoot;
		context.Shell.State.PendingProjectTransition = Ludus::Editor::Core::PendingProjectTransition::OpenProject({ std::move(projectManifest) });

		RefreshContentPanel(projectRoot, context);
	}

	void CloseProjectAction(ProjectSessionCommandContext& context)
	{
		context.Shell.State.PendingProjectTransition = Ludus::Editor::Core::PendingProjectTransition::CloseProject();

		Ludus::Editor::Panels::ClearConsolePanel(context.PanelRegistry);
	}

	void SaveProjectAction(ProjectSessionCommandContext& context)
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

			const auto& scene = context.ProjectSession.RuntimeState.GetEditorScene(editorState.GetActiveSceneId());
			if (Ludus::Editor::Core::SceneQueries::ContainsUnresolvedScriptReferences(
				scene,
				context.ProjectSession.Persistence.GetRuntimeManifest()
			))
			{
				LUDUS_LOG_WARN("The project cannot be saved while the active editor scene contains unresolved script references.");
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
}
