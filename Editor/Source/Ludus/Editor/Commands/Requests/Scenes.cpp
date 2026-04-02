#include "pch.h"

#include <filesystem>
#include <stdexcept>

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/Requests/Scenes.h>
#include <Ludus/Editor/Core/ProjectTemplates.h>
#include <Ludus/Editor/Panels/PanelHelpers.h>
#include <Ludus/Editor/Persistence/ProjectPaths.h>

namespace Ludus::Editor::Commands::Requests::Scenes
{
	namespace
	{
		void RefreshSceneContent(ProjectSessionCommandContext& context)
		{
			Ludus::Editor::Panels::RefreshContentPanel(context.ProjectSession.GetProjectRoot(), context.PanelRegistry);
		}

		std::filesystem::path RequireScenePath(
			Ludus::Engine::Core::SceneHandle sceneHandle,
			ProjectSessionCommandContext& context
		)
		{
			const auto scenePath = context.ProjectSession.TryGetEditorScenePath(sceneHandle);
			if (!scenePath.has_value())
			{
				throw std::runtime_error("No valid path was found for scene.");
			}

			return scenePath.value();
		}

		void SaveManifest(ProjectSessionCommandContext& context)
		{
			context.RuntimeManifestPersistence.Save(
				context.ProjectSession.GetEditorManifest(),
				context.ProjectSession.ProjectManifest.RuntimeManifestPath
			);
		}

		std::optional<std::filesystem::path> TryGetSceneReferencePath(
			Ludus::Engine::Core::SceneHandle sceneHandle,
			ProjectSessionCommandContext& context
		)
		{
			for (const auto& sceneReference : context.ProjectSession.GetEditorManifest().Scenes)
			{
				if (sceneReference.Handle == sceneHandle)
				{
					return sceneReference.Path;
				}
			}

			return std::nullopt;
		}

		void SaveSceneToPath(
			ProjectSessionCommandContext& context,
			Ludus::Engine::Core::SceneHandle sceneHandle,
			const std::filesystem::path& path
		)
		{
			context.ScenePersistence.Save(context.ProjectSession.GetEditorScene(sceneHandle), path);
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
		const auto scenePath = RequireScenePath(command.SceneHandle, context);
		SaveSceneToPath(context, command.SceneHandle, scenePath);

		CommitSceneSave(context, scenePath);
	}

	void SaveSceneAs(const RequestCommand::SaveSceneAs& command, ProjectSessionCommandContext& context)
	{
		const auto& scene = context.ProjectSession.GetEditorScene(command.SceneHandle);
		context.ProjectSession.AddOrUpdateEditorSceneReference(command.SceneHandle, scene.Name, command.Path);

		SaveManifest(context);
		SaveSceneToPath(context, command.SceneHandle, command.Path);

		CommitSceneSave(context, command.Path);
	}

	void RenameScene(const RequestCommand::RenameScene& command, ProjectSessionCommandContext& context)
	{
		auto& scene = context.ProjectSession.GetEditorScene(command.SceneHandle);
		const auto newName = Ludus::Editor::Persistence::ProjectPaths::SceneName(command.Path);
		const auto previousPath = TryGetSceneReferencePath(command.SceneHandle, context);

		context.ProjectSession.AddOrUpdateEditorSceneReference(command.SceneHandle, newName, command.Path);
		scene.Name = newName;

		SaveManifest(context);
		SaveSceneToPath(context, command.SceneHandle, command.Path);

		if (previousPath.has_value() && previousPath.value() != command.Path)
		{
			std::error_code errorCode;
			std::filesystem::remove(previousPath.value(), errorCode);
		}

		CommitSceneSave(context, command.Path);
	}
}
