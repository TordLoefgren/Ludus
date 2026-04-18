#include "pch.h"

#include <filesystem>

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/Requests/SceneActions.h>
#include <Ludus/Editor/Core/ProjectTemplates.h>
#include <Ludus/Editor/Core/SceneQueries.h>
#include <Ludus/Editor/Panels/PanelHelpers.h>
#include <Ludus/Editor/Persistence/ProjectPaths.h>
#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Editor::Commands::Requests::Scenes
{
	namespace
	{
		void RefreshContentPanel(ProjectSessionCommandContext& context)
		{
			Ludus::Editor::Panels::RefreshContentPanel(
				context.ProjectSession.Persistence.GetProjectRoot(),
				context.PanelRegistry
			);
		}

		std::filesystem::path RequireScenePath(
			Ludus::Engine::Core::SceneId id,
			ProjectSessionCommandContext& context
		)
		{
			const auto scenePath = context.ProjectSession.Persistence.TryGetScenePath(id);
			if (!scenePath)
			{
				throw std::runtime_error("No valid path was found for scene.");
			}

			return *scenePath;
		}

		void SaveSceneToPath(
			ProjectSessionCommandContext& context,
			Ludus::Engine::Core::SceneId id,
			const std::filesystem::path& path
		)
		{
			context.Persistence.Scene.Save(context.ProjectSession.RuntimeState.GetEditorScene(id), path);
		}

		void SaveRuntimeManifest(ProjectSessionCommandContext& context)
		{
			context.Persistence.RuntimeManifest.Save(
				context.ProjectSession.Persistence.GetRuntimeManifest(),
				context.ProjectSession.Persistence.GetRuntimeManifestPath()
			);
		}

		void CommitSceneSave(
			ProjectSessionCommandContext& context,
			const std::filesystem::path& path
		)
		{
			context.ProjectSession.EditorState.SetActiveSceneSavePath(path);
			context.ProjectSession.EditorState.SetSceneDirty(false);

			RefreshContentPanel(context);
		}

		void CommitRuntimeManifestSave(ProjectSessionCommandContext& context)
		{
			context.ProjectSession.EditorState.SetRuntimeManifestDirty(false);
		}
	}

	void CreateSceneAction(ProjectSessionCommandContext& context)
	{
		auto scene = Ludus::Editor::Core::ProjectTemplates::CreateDefaultScene();
		context.ProjectSession.SetActiveScene(std::move(scene));
		context.ProjectSession.MarkActiveSceneDirty();
	}

	void CreateSceneAsAction(const std::filesystem::path& path, ProjectSessionCommandContext& context)
	{
		// Remove ".scene.ludus" to get the scene name.
		const auto name = path.filename().stem().stem().string();
		auto scene = Ludus::Editor::Core::ProjectTemplates::CreateDefaultScene(name);
		context.ProjectSession.SetActiveScene(std::move(scene), path);
		context.ProjectSession.MarkActiveSceneDirty();
	}

	void OpenSceneAction(const std::filesystem::path& path, ProjectSessionCommandContext& context)
	{
		auto scene = context.Persistence.Scene.Load(path);
		context.ProjectSession.SetActiveScene(std::move(scene), path);
	}

	void SaveSceneAction(Ludus::Engine::Core::SceneId sceneId, ProjectSessionCommandContext& context)
	{
		const auto& scene = context.ProjectSession.RuntimeState.GetEditorScene(sceneId);
		if (Ludus::Editor::Core::SceneQueries::ContainsUnresolvedScriptReferences(
			scene,
			context.ProjectSession.Persistence.GetRuntimeManifest()
		))
		{
			LUDUS_LOG_WARN("Scene cannot be saved while it contains unresolved script references.");
			return;
		}

		const auto scenePath = RequireScenePath(sceneId, context);
		SaveSceneToPath(context, sceneId, scenePath);
		CommitSceneSave(context, scenePath);
	}

	void SaveSceneAsAction(
		Ludus::Engine::Core::SceneId sceneId,
		const std::filesystem::path& path,
		ProjectSessionCommandContext& context
	)
	{
		const auto& scene = context.ProjectSession.RuntimeState.GetEditorScene(sceneId);
		if (Ludus::Editor::Core::SceneQueries::ContainsUnresolvedScriptReferences(
			scene,
			context.ProjectSession.Persistence.GetRuntimeManifest()
		))
		{
			LUDUS_LOG_WARN("Scene cannot be saved while it contains unresolved script references.");
			return;
		}

		context.ProjectSession.AddOrUpdateSceneReference(scene.Id, scene.Name, path);

		// SaveSceneAs also persists the runtime manifest because the scene reference path has changed.
		SaveSceneToPath(context, sceneId, path);
		SaveRuntimeManifest(context);

		CommitSceneSave(context, path);
		CommitRuntimeManifestSave(context);
	}

	void RenameSceneAction(
		Ludus::Engine::Core::SceneId sceneId,
		const std::filesystem::path& path,
		ProjectSessionCommandContext& context
	)
	{
		auto& scene = context.ProjectSession.RuntimeState.GetEditorScene(sceneId);
		if (Ludus::Editor::Core::SceneQueries::ContainsUnresolvedScriptReferences(
			scene,
			context.ProjectSession.Persistence.GetRuntimeManifest()
		))
		{
			LUDUS_LOG_WARN("Scene cannot be saved while it contains unresolved script references.");
			return;
		}

		const auto newName = Ludus::Editor::Persistence::ProjectPaths::SceneName(path);
		const auto previousPath = context.ProjectSession.Persistence.TryGetScenePath(sceneId);

		context.ProjectSession.AddOrUpdateSceneReference(scene.Id, newName, path);
		scene.Name = newName;

		// RenameScene also persists the runtime manifest because the scene reference path has changed.
		SaveSceneToPath(context, sceneId, path);
		SaveRuntimeManifest(context);

		if (previousPath && *previousPath != path)
		{
			std::error_code errorCode;
			if (!std::filesystem::remove(*previousPath, errorCode))
			{
				LUDUS_LOG_WARN("Failed to remove previous scene: " + errorCode.message());
			}
		}

		CommitSceneSave(context, path);
		CommitRuntimeManifestSave(context);
	}
}
