#include "pch.h"

#include <filesystem>
#include <format>
#include <string>
#include <string_view>

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/Requests/ProjectTransitionContext.h>
#include <Ludus/Editor/Core/AssetRefresh.h>
#include <Ludus/Editor/Core/ProjectTemplates.h>
#include <Ludus/Editor/Core/RecentlyOpenedProject.h>
#include <Ludus/Editor/Core/SceneQueries.h>
#include <Ludus/Editor/Panels/PanelHelpers.h>
#include <Ludus/Editor/Persistence/ProjectPaths.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Persistence/Paths.h>

namespace Ludus::Editor::Commands::Requests::Projects
{
	namespace
	{
		void CreateProject(
			const std::filesystem::path& projectRoot,
			std::string_view projectName,
			Ludus::Editor::Core::PendingProjectTransition& pendingProjectTransition,
			Ludus::Editor::Core::EditorPersistenceContext& persistence
		)
		{
			Ludus::Editor::Persistence::ProjectPaths::EnsureProjectLayoutExists(projectRoot);

			// Create default scene.
			const auto scene = Ludus::Editor::Core::ProjectTemplates::CreateDefaultScene();
			const auto scenePath = Ludus::Editor::Persistence::ProjectPaths::SceneFile(projectRoot, scene.Name);
			const auto relativeScenePath =
				Ludus::Engine::Persistence::Paths::NormalizeRuntimeScenePathOrEmpty(
					projectRoot,
					scenePath
				);
			persistence.Scene.Save(
				scene,
				scenePath
			);

			// Create runtime manifest.
			const auto runtimeManifest = Ludus::Engine::Runtime::RuntimeManifest::Create(
				scene.Id,
				{ { scene.Id, scene.Name, relativeScenePath } },
				{ }
			);
			const auto runtimeManifestPath = Ludus::Engine::Persistence::Paths::RuntimeManifestFile(projectRoot, projectName);
			persistence.RuntimeManifest.Save(
				runtimeManifest,
				runtimeManifestPath
			);

			// Create runtime settings.
			const auto runtimeLaunchSettings = Ludus::Engine::Runtime::RuntimeLaunchSettings();
			const auto runtimeLaunchSettingsPath = Ludus::Engine::Persistence::Paths::RuntimeLaunchSettingsFile(projectRoot, projectName);
			persistence.RuntimeLaunchSettings.Save(
				runtimeLaunchSettings,
				runtimeLaunchSettingsPath
			);

			// Create project manifest.
			const auto projectManifest = Ludus::Editor::Core::ProjectManifest::Create(
				projectRoot,
				runtimeManifestPath
			);
			persistence.ProjectManifest.Save(
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

		void RefreshContentPanel(ProjectSessionCommandContext& context)
		{
			Ludus::Editor::Panels::RefreshContentPanel(
				context.ProjectSession.Persistence.GetProjectRoot(),
				context.PanelRegistry
			);
		}

		void LogAssetRefreshSummary(const Ludus::Editor::Core::AssetRefreshSummary& summary)
		{
			LUDUS_LOG_INFO(std::format(
				"Asset refresh: {} registered, {} candidates, {} missing source, {} unsupported.",
				summary.RegisteredCount,
				summary.CandidateCount,
				summary.MissingSourceCount,
				summary.UnsupportedCount
			));
		}

		void RefreshRecentlyOpenedProjects(
			const std::filesystem::path& projectRoot,
			std::string_view projectName,
			ProjectTransitionContext context
		)
		{
			context.Preferences.AddRecentlyOpenedProject(
				projectName,
				Ludus::Editor::Persistence::ProjectPaths::ProjectManifestFile(projectRoot, projectName)
			);
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
			context.Persistence
		);

		RefreshContentPanel(projectRoot, context);
		RefreshRecentlyOpenedProjects(projectRoot, name, context);
	}

	void CreateProjectAsAction(const std::filesystem::path& path, const std::string& name, ProjectTransitionContext context)
	{
		Ludus::Editor::Persistence::ProjectPaths::EnsureProjectsRootExists();

		CreateProject(
			path,
			name,
			context.Shell.State.PendingProjectTransition,
			context.Persistence
		);

		RefreshContentPanel(path, context);
		RefreshRecentlyOpenedProjects(path, name, context);
	}

	void OpenProjectAction(const std::filesystem::path& path, ProjectTransitionContext context)
	{
		auto projectManifest = context.Persistence.ProjectManifest.Load(path);
		const auto projectRoot = projectManifest.ProjectRoot;
		const auto projectName = projectManifest.RuntimeManifestPath.filename().stem().stem().string();

		context.Shell.State.PendingProjectTransition = Ludus::Editor::Core::PendingProjectTransition::OpenProject({ std::move(projectManifest) });

		RefreshContentPanel(projectRoot, context);
		RefreshRecentlyOpenedProjects(projectRoot, projectName, context);
	}

	void CloseProjectAction(ProjectSessionCommandContext& context)
	{
		context.Shell.State.PendingProjectTransition = Ludus::Editor::Core::PendingProjectTransition::CloseProject();

		Ludus::Editor::Panels::ClearConsolePanel(context.PanelRegistry);
	}

	void SaveProjectAction(ProjectSessionCommandContext& context)
	{
		auto& editorState = context.ProjectSession.EditorState;
		auto& persistence = context.ProjectSession.Persistence;
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
			context.Persistence.Scene.Save(
				context.ProjectSession.RuntimeState.GetEditorScene(activeSceneId),
				Ludus::Engine::FileSystem::ResolvePathFromRoot(
					persistence.GetProjectRoot(),
					editorState.GetActiveSceneSavePath()
				)
			);

			editorState.SetSceneDirty(false);
		}

		if (editorState.IsProjectManifestDirty())
		{
			context.Persistence.ProjectManifest.Save(
				persistence.GetProjectManifest(),
				persistence.GetProjectManifestPath()
			);

			editorState.SetProjectManifestDirty(false);
		}

		if (editorState.IsRuntimeManifestDirty())
		{
			context.Persistence.RuntimeManifest.Save(
				persistence.GetRuntimeManifest(),
				persistence.GetRuntimeManifestPath()
			);

			editorState.SetRuntimeManifestDirty(false);
		}

		if (editorState.IsRuntimeLaunchSettingsDirty())
		{
			context.Persistence.RuntimeLaunchSettings.Save(
				persistence.GetRuntimeLaunchSettings(),
				persistence.GetRuntimeLaunchSettingsPath()
			);

			editorState.SetRuntimeLaunchSettingsDirty(false);
		}
	}

	void RefreshAssetsAction(ProjectSessionCommandContext& context)
	{
		LogAssetRefreshSummary(context.ProjectSession.RefreshAssets());
		RefreshContentPanel(context);
	}

	void IncludeAssetAction(const std::filesystem::path& path, ProjectSessionCommandContext& context)
	{
		const auto classification = Ludus::Editor::Core::TryClassifyAssetFile(
			context.ProjectSession.Persistence,
			path
		);
		if (!classification || classification->Classification != Ludus::Editor::Core::AssetRefreshClassification::Candidate)
		{
			throw std::runtime_error("Asset is no longer a candidate for inclusion: " + path.string());
		}

		if (classification->Type != Ludus::Engine::Core::AssetType::Texture2D)
		{
			throw std::runtime_error("Only candidate texture assets can be included: " + path.string());
		}

		if (!context.ProjectSession.IncludeAsset(classification->Type, path))
		{
			throw std::runtime_error("Asset could not be included in the project: " + path.string());
		}

		RefreshContentPanel(context);
	}
}
