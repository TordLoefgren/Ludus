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
		void RefreshSceneContent(ProjectSessionCommandContext& context)
		{
			Ludus::Editor::Panels::RefreshContentPanel(context.ProjectSession.GetProjectRoot(), context.PanelRegistry);
		}

		std::filesystem::path RequireScenePath(
			Ludus::Engine::Core::SceneId sceneId,
			ProjectSessionCommandContext& context
		)
		{
			const auto scenePath = context.ProjectSession.TryGetEditorScenePath(sceneId);
			if (!scenePath)
			{
				throw std::runtime_error("No valid path was found for scene.");
			}

			return *scenePath;
		}

		void SaveManifest(ProjectSessionCommandContext& context)
		{
			context.RuntimeManifestPersistence.Save(
				context.ProjectSession.GetEditorManifest(),
				context.ProjectSession.ProjectManifest.RuntimeManifestPath
			);
		}

		std::optional<std::filesystem::path> TryGetSceneReferencePath(
			Ludus::Engine::Core::SceneId sceneId,
			ProjectSessionCommandContext& context
		)
		{
			for (const auto& sceneReference : context.ProjectSession.GetEditorManifest().Scenes)
			{
				if (sceneReference.Id == sceneId)
				{
					return sceneReference.Path;
				}
			}

			return std::nullopt;
		}

		void SaveSceneToPath(
			ProjectSessionCommandContext& context,
			Ludus::Engine::Core::SceneId sceneId,
			const std::filesystem::path& path
		)
		{
			context.ScenePersistence.Save(context.ProjectSession.GetEditorScene(sceneId), path);
		}

		void CommitSceneSave(
			ProjectSessionCommandContext& context,
			const std::filesystem::path& path
		)
		{
			context.ProjectSession.SetActiveSceneSavePath(path);
			context.ProjectSession.ClearSceneDirty();
			RefreshSceneContent(context);
		}
	}

	void CreateScene(ProjectSessionCommandContext& context)
	{
		auto scene = Ludus::Editor::Core::ProjectTemplates::CreateDefaultScene();
		context.ProjectSession.SetActiveScene(std::move(scene));
		context.ProjectSession.MarkSceneDirty();
	}

	void CreateSceneAs(const RequestCommand::CreateSceneAs& command, ProjectSessionCommandContext& context)
	{
		auto scene = Ludus::Editor::Core::ProjectTemplates::CreateDefaultScene();
		context.ProjectSession.SetActiveScene(std::move(scene), command.Path);
	}

	void OpenScene(const RequestCommand::OpenScene& command, ProjectSessionCommandContext& context)
	{
		auto scene = context.ScenePersistence.Load(command.Path);
		context.ProjectSession.SetActiveScene(std::move(scene), command.Path);
	}

	void SaveScene(const RequestCommand::SaveScene& command, ProjectSessionCommandContext& context)
	{
		const auto scenePath = RequireScenePath(command.SceneId, context);
		SaveSceneToPath(context, command.SceneId, scenePath);

		CommitSceneSave(context, scenePath);
	}

	void SaveSceneAs(const RequestCommand::SaveSceneAs& command, ProjectSessionCommandContext& context)
	{
		const auto& scene = context.ProjectSession.GetEditorScene(command.SceneId);
		context.ProjectSession.AddOrUpdateEditorSceneReference(scene.Id, scene.Name, command.Path);

		SaveManifest(context);
		SaveSceneToPath(context, command.SceneId, command.Path);

		CommitSceneSave(context, command.Path);
	}

	void RenameScene(const RequestCommand::RenameScene& command, ProjectSessionCommandContext& context)
	{
		auto& scene = context.ProjectSession.GetEditorScene(command.SceneId);
		const auto newName = Ludus::Editor::Persistence::ProjectPaths::SceneName(command.Path);
		const auto previousPath = TryGetSceneReferencePath(command.SceneId, context);

		context.ProjectSession.AddOrUpdateEditorSceneReference(scene.Id, newName, command.Path);
		scene.Name = newName;

		SaveManifest(context);
		SaveSceneToPath(context, command.SceneId, command.Path);

		if (previousPath && *previousPath != command.Path)
		{
			std::error_code errorCode;
			std::filesystem::remove(*previousPath, errorCode);
		}

		CommitSceneSave(context, command.Path);
	}
}
