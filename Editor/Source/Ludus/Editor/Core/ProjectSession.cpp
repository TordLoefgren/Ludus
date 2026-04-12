#include "pch.h"

#include <filesystem>
#include <optional>
#include <stdexcept>
#include <utility>

#include <Ludus/Editor/Core/ProjectSession.h>
#include <Ludus/Editor/Core/ProjectSessionEditorState.h>
#include <Ludus/Editor/Core/ProjectSessionPersistence.h>
#include <Ludus/Editor/Core/ProjectSessionRuntimeState.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Runtime/RuntimeInstanceBuilder.h>

namespace Ludus::Editor::Core
{
	ProjectSession ProjectSession::Create(
		Ludus::Editor::Core::ProjectManifest projectManifest,
		std::unique_ptr<Ludus::Engine::Runtime::RuntimeInstance> editorRuntime,
		Ludus::Engine::Runtime::RuntimeLaunchSettings runtimeLaunchSettings,
		Ludus::Engine::Core::SceneId activeSceneId
	)
	{
		if (!editorRuntime)
		{
			throw std::runtime_error("ProjectSession requires an editor runtime.");
		}

		if (!editorRuntime->GetSceneRegistry().Contains(activeSceneId))
		{
			throw std::runtime_error("ProjectSession requires an active scene to exist in the editor runtime.");
		}

		const auto& runtimeManifest = editorRuntime->GetRuntimeManifest();

		std::optional<std::filesystem::path> activeSceneSavePath;
		for (const auto& sceneReference : runtimeManifest.Scenes)
		{
			if (sceneReference.Id == activeSceneId)
			{
				activeSceneSavePath = sceneReference.Path;
				break;
			}
		}

		auto runtimeManifestCopy = Ludus::Engine::Runtime::RuntimeManifest::Create(
			runtimeManifest.EntrySceneId,
			runtimeManifest.Scenes,
			runtimeManifest.Scripts
		);

		return ProjectSession(
			ProjectSessionPersistence::Create(std::move(projectManifest), std::move(runtimeManifestCopy), std::move(runtimeLaunchSettings)),
			ProjectSessionEditorState::Create(activeSceneId, std::move(activeSceneSavePath)),
			ProjectSessionRuntimeState::Create(std::move(editorRuntime))
		);
	}

	void ProjectSession::SetActiveScene(Ludus::Engine::Core::Scene scene, std::optional<std::filesystem::path> sceneSavePath)
	{
		auto& sceneRegistry = RuntimeState.GetEditorSceneRegistry();
		sceneRegistry.Clear();

		const auto sceneId = scene.Id;
		sceneRegistry.AddScene(std::move(scene));

		EditorState.SetActiveScene(sceneId, sceneSavePath ? sceneSavePath : Persistence.TryGetScenePath(sceneId));
		RuntimeState.GetEditorSceneRuntimeState().Presentation.CurrentSceneId = sceneId;
	}

	Ludus::Engine::Core::SceneId ProjectSession::GetPresentedSceneId() const
	{
		if (RuntimeState.IsSimulationActive())
		{
			return RuntimeState.GetActiveSceneRuntimeState().Presentation.CurrentSceneId;
		}

		return EditorState.GetActiveSceneId();
	}

	void ProjectSession::StartSimulation(Ludus::Engine::Runtime::IHostContext& hostContext)
	{
		RuntimeState.StartSimulation(
			hostContext,
			Persistence.GetRuntimeManifest(),
			EditorState.GetActiveSceneId()
		);
	}

	void ProjectSession::PauseSimulation(Ludus::Engine::Runtime::IHostContext& hostContext)
	{
		RuntimeState.PauseSimulation(hostContext);
	}

	void ProjectSession::StopSimulation(Ludus::Engine::Runtime::IHostContext& hostContext)
	{
		RuntimeState.StopSimulation(hostContext);
	}

	void ProjectSession::ShutdownRuntimes()
	{
		RuntimeState.ShutdownRuntimes();
	}

	bool ProjectSession::UpdateRuntimeLaunchSettings(const Ludus::Engine::Runtime::RuntimeLaunchSettings& runtimeLaunchSettings)
	{
		const auto changed = Persistence.UpdateRuntimeLaunchSettings(runtimeLaunchSettings);
		if (changed)
		{
			RuntimeState.ApplyRuntimeLaunchSettings(runtimeLaunchSettings);
			EditorState.SetRuntimeLaunchSettingsDirty(true);
		}

		return changed;
	}

	bool ProjectSession::AddOrUpdateSceneReference(
		Ludus::Engine::Core::SceneId id,
		std::string name,
		std::filesystem::path path
	)
	{
		const auto changed = Persistence.AddOrUpdateSceneReference(id, name, path);
		if (changed)
		{
			EditorState.SetRuntimeManifestDirty(true);
		}

		return changed;
	}

	bool ProjectSession::AddOrUpdateScriptReference(Ludus::Engine::Core::ScriptId id, std::string name)
	{
		const auto changed = Persistence.AddOrUpdateScriptReference(id, name);
		if (changed)
		{
			EditorState.SetRuntimeManifestDirty(true);
		}

		return changed;
	}

	bool ProjectSession::RemoveScriptReference(Ludus::Engine::Core::ScriptId id)
	{
		const auto changed = Persistence.RemoveScriptReference(id);
		if (changed)
		{
			EditorState.SetRuntimeManifestDirty(true);
		}

		return changed;
	}

	void ProjectSession::MarkActiveSceneDirty()
	{
		if (!RuntimeState.IsSimulationActive())
		{
			EditorState.SetSceneDirty(true);
		}
	}
}
