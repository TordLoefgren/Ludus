#include "pch.h"

#include <filesystem>

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/Requests/Scenes.h>
#include <Ludus/Editor/Core/ProjectTemplates.h>
#include <Ludus/Editor/Panels/PanelHelpers.h>

namespace Ludus::Editor::Commands::Requests::Scenes
{
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
		const auto& scene = context.ProjectSession.GetEditorScene(command.SceneHandle);
		const auto& scenePath = context.ProjectSession.TryGetEditorScenePath(command.SceneHandle);
		if (!scenePath.has_value())
		{
			throw std::runtime_error("No valid path was found for scene.");
		}

		context.ScenePersistence.Save(scene, scenePath.value());
		context.ProjectSession.ClearSceneDirty();

		Ludus::Editor::Panels::RefreshContentPanel(context.ProjectSession.GetProjectRoot(), context.PanelRegistry);
	}

	void SaveSceneAs(const RequestCommand::SaveSceneAs& command, ProjectSessionCommandContext& context)
	{
		const auto& scene = context.ProjectSession.GetEditorScene(command.SceneHandle);

		auto& runtimeManifest = context.ProjectSession.GetEditorManifest();

		auto hasSceneReference = false;
		for (auto& sceneReference : runtimeManifest.Scenes)
		{
			if (sceneReference.Handle == command.SceneHandle)
			{
				sceneReference.Name = scene.Name;
				sceneReference.Path = command.Path;
				hasSceneReference = true;
				break;
			}
		}

		if (!hasSceneReference)
		{
			runtimeManifest.Scenes.push_back({ command.SceneHandle, scene.Name, command.Path });
		}

		context.RuntimeManifestPersistence.Save(runtimeManifest, context.ProjectSession.ProjectManifest.RuntimeManifestPath);
		context.ScenePersistence.Save(scene, command.Path);

		context.ProjectSession.SetActiveSceneSavePath(command.Path);
		context.ProjectSession.ClearSceneDirty();

		Ludus::Editor::Panels::RefreshContentPanel(context.ProjectSession.GetProjectRoot(), context.PanelRegistry);
	}
}
