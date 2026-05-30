#include "pch.h"

#include <filesystem>

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/Requests/SceneActions.h>
#include <Ludus/Editor/Core/ProjectTemplates.h>
#include <Ludus/Editor/Core/SceneQueries.h>
#include <Ludus/Editor/Panels/PanelHelpers.h>
#include <Ludus/Editor/Persistence/ProjectPaths.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Persistence/Paths.h>

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

		std::filesystem::path RequireResolvedScenePath(
			Ludus::Engine::Core::SceneId id,
			ProjectSessionCommandContext& context
		)
		{
			return Ludus::Engine::FileSystem::ResolvePathFromRoot(
				context.ProjectSession.Persistence.GetProjectRoot(),
				RequireScenePath(id, context)
			);
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

		bool ContainsUnresolvedReferences(
			const Ludus::Engine::Core::Scene& scene,
			const Ludus::Engine::Runtime::RuntimeManifest& runtimeManifest,
			std::string& errorMessage
		)
		{
			if (Ludus::Editor::Core::SceneQueries::ContainsUnresolvedAssetReferences(
				scene,
				runtimeManifest
			))
			{
				errorMessage = "Scene cannot be saved while it contains unresolved asset references.";
				return true;
			}

			if (Ludus::Editor::Core::SceneQueries::ContainsUnresolvedScriptReferences(
				scene,
				runtimeManifest
			))
			{
				errorMessage = "Scene cannot be saved while it contains unresolved script references.";
				return true;
			}

			return false;
		}

		bool TryNormalizeScenePathRelativeToProject(
			const std::filesystem::path& path,
			ProjectSessionCommandContext& context,
			std::filesystem::path& out
		)
		{
			out = Ludus::Engine::Persistence::Paths::NormalizeRuntimeScenePathOrEmpty(
				context.ProjectSession.Persistence.GetProjectRoot(),
				path
			);
			if (!out.empty())
			{
				return true;
			}

			LUDUS_LOG_WARN("Scene path must be inside the project Scenes directory.");
			return false;
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
		std::filesystem::path relativePath;
		if (!TryNormalizeScenePathRelativeToProject(path, context, relativePath))
		{
			return;
		}

		// Remove ".scene.ludus" to get the scene name.
		const auto name = path.filename().stem().stem().string();
		auto scene = Ludus::Editor::Core::ProjectTemplates::CreateDefaultScene(name);
		context.ProjectSession.SetActiveScene(std::move(scene), relativePath);
		context.ProjectSession.MarkActiveSceneDirty();
	}

	void OpenSceneAction(const std::filesystem::path& path, ProjectSessionCommandContext& context)
	{
		std::filesystem::path relativePath;
		if (!TryNormalizeScenePathRelativeToProject(path, context, relativePath))
		{
			return;
		}

		auto scene = context.Persistence.Scene.Load(
			Ludus::Engine::FileSystem::ResolvePathFromRoot(
				context.ProjectSession.Persistence.GetProjectRoot(),
				relativePath
			)
		);
		context.ProjectSession.SetActiveScene(std::move(scene), relativePath);
	}

	void SaveSceneAction(Ludus::Engine::Core::SceneId sceneId, ProjectSessionCommandContext& context)
	{
		const auto& scene = context.ProjectSession.RuntimeState.GetEditorScene(sceneId);
		const auto& runtimeManifest = context.ProjectSession.Persistence.GetRuntimeManifest();
		std::string errorMessage;
		if (ContainsUnresolvedReferences(scene, runtimeManifest, errorMessage))
		{
			LUDUS_LOG_WARN(errorMessage);
			return;
		}

		const auto scenePath = RequireScenePath(sceneId, context);
		SaveSceneToPath(context, sceneId, RequireResolvedScenePath(sceneId, context));
		CommitSceneSave(context, scenePath);
	}

	void SaveSceneAsAction(
		Ludus::Engine::Core::SceneId sceneId,
		const std::filesystem::path& path,
		ProjectSessionCommandContext& context
	)
	{
		const auto& scene = context.ProjectSession.RuntimeState.GetEditorScene(sceneId);
		const auto& runtimeManifest = context.ProjectSession.Persistence.GetRuntimeManifest();
		std::string errorMessage;
		if (ContainsUnresolvedReferences(scene, runtimeManifest, errorMessage))
		{
			LUDUS_LOG_WARN(errorMessage);
			return;
		}

		std::filesystem::path relativePath;
		if (!TryNormalizeScenePathRelativeToProject(path, context, relativePath))
		{
			return;
		}

		context.ProjectSession.AddOrUpdateSceneReference(scene.Id, scene.Name, relativePath);

		// SaveSceneAs also persists the runtime manifest because the scene reference path has changed.
		SaveSceneToPath(
			context,
			sceneId,
			Ludus::Engine::FileSystem::ResolvePathFromRoot(
				context.ProjectSession.Persistence.GetProjectRoot(),
				relativePath
			)
		);
		SaveRuntimeManifest(context);

		CommitSceneSave(context, relativePath);
		CommitRuntimeManifestSave(context);
	}

	void RenameSceneAction(
		Ludus::Engine::Core::SceneId sceneId,
		const std::filesystem::path& path,
		ProjectSessionCommandContext& context
	)
	{
		auto& scene = context.ProjectSession.RuntimeState.GetEditorScene(sceneId);
		const auto& runtimeManifest = context.ProjectSession.Persistence.GetRuntimeManifest();
		std::string errorMessage;
		if (ContainsUnresolvedReferences(scene, runtimeManifest, errorMessage))
		{
			LUDUS_LOG_WARN(errorMessage);
			return;
		}

		std::filesystem::path relativePath;
		if (!TryNormalizeScenePathRelativeToProject(path, context, relativePath))
		{
			return;
		}

		const auto newName = Ludus::Editor::Persistence::ProjectPaths::SceneName(relativePath);
		const auto previousPath = context.ProjectSession.Persistence.TryGetScenePath(sceneId);

		context.ProjectSession.AddOrUpdateSceneReference(scene.Id, newName, relativePath);
		scene.Name = newName;

		// RenameScene also persists the runtime manifest because the scene reference path has changed.
		SaveSceneToPath(
			context,
			sceneId,
			Ludus::Engine::FileSystem::ResolvePathFromRoot(
				context.ProjectSession.Persistence.GetProjectRoot(),
				relativePath
			)
		);
		SaveRuntimeManifest(context);

		if (previousPath && *previousPath != relativePath)
		{
			std::error_code errorCode;
			if (!std::filesystem::remove(
				Ludus::Engine::FileSystem::ResolvePathFromRoot(
					context.ProjectSession.Persistence.GetProjectRoot(),
					*previousPath
				),
				errorCode
			))
			{
				LUDUS_LOG_WARN("Failed to remove previous scene: " + errorCode.message());
			}
		}

		CommitSceneSave(context, relativePath);
		CommitRuntimeManifestSave(context);
	}
}
