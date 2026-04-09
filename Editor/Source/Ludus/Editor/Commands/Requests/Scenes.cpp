#include "pch.h"

#include <filesystem>
#include <stdexcept>

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/Requests/Scenes.h>
#include <Ludus/Editor/Core/ProjectTemplates.h>
#include <Ludus/Editor/Panels/PanelHelpers.h>
#include <Ludus/Editor/Persistence/ProjectPaths.h>
#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Editor::Commands::Requests::Scenes
{
	namespace
	{
		bool CanOpenScene(ProjectSessionCommandContext& context)
		{
			if (!context.ProjectSession.EditorState.HasUnsavedChanges())
			{
				return true;
			}

			LUDUS_LOG_WARN("Open scene cannot be executed while the project has unsaved changes.");
			return false;
		}

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

		bool CanSaveScene(
			ProjectSessionCommandContext& context,
			Ludus::Engine::Core::SceneId id
		)
		{
			const auto& scene = context.ProjectSession.RuntimeState.GetEditorScene(id);
			const auto& runtimeManifest = context.ProjectSession.Persistence.GetRuntimeManifest();
			if (HasUnresolvedScriptReferences(scene, runtimeManifest))
			{
				LUDUS_LOG_WARN("Scene cannot be saved while it contains unresolved script references.");
				return false;
			}

			return true;
		}

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
			context.ScenePersistence.Save(context.ProjectSession.RuntimeState.GetEditorScene(id), path);
		}

		void SaveRuntimeManifest(ProjectSessionCommandContext& context)
		{
			context.RuntimeManifestPersistence.Save(
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

	void CreateScene(ProjectSessionCommandContext& context)
	{
		auto scene = Ludus::Editor::Core::ProjectTemplates::CreateDefaultScene();
		context.ProjectSession.SetActiveScene(std::move(scene));
		context.ProjectSession.MarkActiveSceneDirty();
	}

	void CreateSceneAs(const RequestCommand::CreateSceneAs& command, ProjectSessionCommandContext& context)
	{
		auto scene = Ludus::Editor::Core::ProjectTemplates::CreateDefaultScene();
		context.ProjectSession.SetActiveScene(std::move(scene), command.Path);
		context.ProjectSession.MarkActiveSceneDirty();
	}

	void OpenScene(const RequestCommand::OpenScene& command, ProjectSessionCommandContext& context)
	{
		if (!CanOpenScene(context))
		{
			return;
		}

		auto scene = context.ScenePersistence.Load(command.Path);
		context.ProjectSession.SetActiveScene(std::move(scene), command.Path);
	}

	void SaveScene(const RequestCommand::SaveScene& command, ProjectSessionCommandContext& context)
	{
		if (!CanSaveScene(context, command.SceneId))
		{
			return;
		}

		const auto scenePath = RequireScenePath(command.SceneId, context);
		SaveSceneToPath(context, command.SceneId, scenePath);
		CommitSceneSave(context, scenePath);
	}

	void SaveSceneAs(const RequestCommand::SaveSceneAs& command, ProjectSessionCommandContext& context)
	{
		if (!CanSaveScene(context, command.SceneId))
		{
			return;
		}

		const auto& scene = context.ProjectSession.RuntimeState.GetEditorScene(command.SceneId);
		context.ProjectSession.AddOrUpdateSceneReference(scene.Id, scene.Name, command.Path);

		SaveSceneToPath(context, command.SceneId, command.Path);
		// SaveSceneAs also persists the runtime manifest because the scene reference path changed.
		SaveRuntimeManifest(context);
		CommitSceneSave(context, command.Path);
		CommitRuntimeManifestSave(context);
	}

	void RenameScene(const RequestCommand::RenameScene& command, ProjectSessionCommandContext& context)
	{
		if (!CanSaveScene(context, command.SceneId))
		{
			return;
		}

		auto& scene = context.ProjectSession.RuntimeState.GetEditorScene(command.SceneId);
		const auto newName = Ludus::Editor::Persistence::ProjectPaths::SceneName(command.Path);
		const auto previousPath = context.ProjectSession.Persistence.TryGetScenePath(command.SceneId);

		context.ProjectSession.AddOrUpdateSceneReference(scene.Id, newName, command.Path);
		scene.Name = newName;

		SaveSceneToPath(context, command.SceneId, command.Path);
		// RenameScene also persists the runtime manifest so scene references stay aligned with the renamed file.
		SaveRuntimeManifest(context);

		if (previousPath && *previousPath != command.Path)
		{
			std::error_code errorCode;
			if (!std::filesystem::remove(*previousPath, errorCode))
			{
				LUDUS_LOG_WARN("Failed to remove previous scene: " + errorCode.message());
			}
		}

		CommitSceneSave(context, command.Path);
		CommitRuntimeManifestSave(context);
	}
}
