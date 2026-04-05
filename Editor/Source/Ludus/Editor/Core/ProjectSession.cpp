#include "pch.h"

#include <filesystem>
#include <optional>
#include <stdexcept>
#include <utility>

#include <Ludus/Editor/Core/ProjectSession.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Core/Random.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Runtime/RuntimeInstanceBuilder.h>

namespace Ludus::Editor::Core
{
	ProjectSession ProjectSession::Create(
		Ludus::Editor::Core::ProjectManifest projectManifest,
		std::unique_ptr<Ludus::Engine::Runtime::RuntimeInstance> editorRuntime,
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

		return {
			.ProjectManifest = std::move(projectManifest),
			.EditorManifest = Ludus::Engine::Runtime::RuntimeManifest::Create(
				runtimeManifest.EntrySceneId,
				runtimeManifest.Scenes,
				runtimeManifest.Scripts
			),
			.EditorRuntime = std::move(editorRuntime),
			.SimulationRuntime = nullptr,
			.EditorState = Ludus::Editor::Core::ProjectSessionEditorState::Create(
				activeSceneId,
				std::move(activeSceneSavePath)
			)
		};
	}

	void ProjectSession::SetActiveScene(Ludus::Engine::Core::Scene scene, std::optional<std::filesystem::path> sceneSavePath)
	{
		auto& sceneRegistry = GetEditorRuntime().GetSceneRegistry();
		sceneRegistry.Clear();

		const auto sceneId = scene.Id;
		sceneRegistry.AddScene(std::move(scene));

		EditorState.ActiveSceneId = sceneId;
		EditorState.ActiveSceneState = {
			.IsDirty = false,
			.SavePath = sceneSavePath ? sceneSavePath : TryGetEditorScenePath(sceneId)
		};
		GetEditorRuntime().GetScenePresentationState().CurrentSceneId = sceneId;
	}

	void ProjectSession::StartSimulation(Ludus::Engine::Runtime::IHostContext& hostContext)
	{
		if (SimulationRuntime)
		{
			hostContext.AttachRuntime(SimulationRuntime.get());
			return;
		}

		const auto& editorRuntime = *EditorRuntime;
		const auto& editorManifest = GetEditorManifest();
		const auto activeSceneId = EditorState.ActiveSceneId;
		const auto& activeScene = editorRuntime.GetSceneRegistry().GetScene(activeSceneId);

		auto simulationRuntime = Ludus::Engine::Runtime::RuntimeInstanceBuilder::Create()
			.UseDefaultPhysics2D()
			.UseDefaultRendering2D()
			.UseDefaultScripting()
			.WithRuntimeManifest(editorManifest)
			.WithRuntimeEnvironment(editorRuntime.GetRuntimeEnvironment())
			.WithExplicitScene(Ludus::Engine::Core::Scene::Clone(activeScene))
			.Build(hostContext);

		SimulationRuntime = std::move(simulationRuntime);

		hostContext.AttachRuntime(SimulationRuntime.get());
	}

	void ProjectSession::PauseSimulation(Ludus::Engine::Runtime::IHostContext& hostContext)
	{
		if (!SimulationRuntime)
		{
			return;
		}

		hostContext.AttachRuntime(SimulationRuntime.get());
	}

	void ProjectSession::StopSimulation(Ludus::Engine::Runtime::IHostContext& hostContext)
	{
		if (SimulationRuntime)
		{
			hostContext.AttachRuntime(EditorRuntime.get());
			SimulationRuntime->Shutdown();
			SimulationRuntime.reset();

			return;
		}

		hostContext.AttachRuntime(EditorRuntime.get());
	}

	void ProjectSession::ShutdownRuntimes()
	{
		if (SimulationRuntime)
		{
			SimulationRuntime->Shutdown();
			SimulationRuntime.reset();
		}

		if (EditorRuntime)
		{
			EditorRuntime->Shutdown();
		}
	}

	const Ludus::Engine::Runtime::RuntimeManifest& ProjectSession::GetEditorManifest() const
	{
		return EditorManifest;
	}

	Ludus::Engine::Runtime::RuntimeManifest& ProjectSession::GetEditorManifest()
	{
		return EditorManifest;
	}

	Ludus::Engine::Core::Scene& ProjectSession::GetEditorScene(Ludus::Engine::Core::SceneId sceneId)
	{
		return GetEditorRuntime().GetSceneRegistry().GetScene(sceneId);
	}

	const Ludus::Engine::Core::Scene& ProjectSession::GetEditorScene(Ludus::Engine::Core::SceneId sceneId) const
	{
		return GetEditorRuntime().GetSceneRegistry().GetScene(sceneId);
	}

	std::optional<std::filesystem::path> ProjectSession::TryGetEditorScenePath(Ludus::Engine::Core::SceneId sceneId) const
	{
		std::filesystem::path scenePath;

		for (const auto& sceneReference : GetEditorManifest().Scenes)
		{
			if (sceneReference.Id == sceneId)
			{
				scenePath = sceneReference.Path;
				break;
			}
		}

		if (scenePath.empty())
		{
			return std::nullopt;
		}

		return scenePath;
	}

	void ProjectSession::AddOrUpdateEditorSceneReference(
		Ludus::Engine::Core::SceneId id,
		std::string name,
		std::filesystem::path path
	)
	{
		for (auto& sceneReference : GetEditorManifest().Scenes)
		{
			if (sceneReference.Id == id)
			{
				sceneReference.Name = std::move(name);
				sceneReference.Path = std::move(path);

				return;
			}
		}

		GetEditorManifest().Scenes.push_back({ id, std::move(name), std::move(path) });
	}

	bool ProjectSession::HasEditorScriptReference(Ludus::Engine::Core::ScriptId id) const
	{
		for (const auto& scriptReference : GetEditorManifest().Scripts)
		{
			if (scriptReference.Id == id)
			{
				return true;
			}
		}

		return false;
	}

	bool ProjectSession::HasEditorScriptReference(std::string_view name) const
	{
		for (const auto& scriptReference : GetEditorManifest().Scripts)
		{
			if (scriptReference.Name == name)
			{
				return true;
			}
		}

		return false;
	}

	bool ProjectSession::AddOrUpdateEditorScriptReference(
		Ludus::Engine::Core::ScriptId id,
		std::string name
	)
	{
		for (auto& scriptReference : GetEditorManifest().Scripts)
		{
			if (scriptReference.Id == id)
			{
				if (scriptReference.Name == name)
				{
					return false;
				}

				scriptReference.Name = std::move(name);
				return true;
			}
		}

		GetEditorManifest().Scripts.push_back({ id, std::move(name) });
		return true;
	}

	bool ProjectSession::RemoveEditorScriptReference(Ludus::Engine::Core::ScriptId id)
	{
		auto& scripts = GetEditorManifest().Scripts;
		for (auto iter = scripts.begin(); iter != scripts.end(); ++iter)
		{
			if (iter->Id == id)
			{
				scripts.erase(iter);
				return true;
			}
		}

		return false;
	}

	Ludus::Engine::Core::ScriptId ProjectSession::AllocateEditorScriptId() const
	{
		auto random = Ludus::Engine::Core::Random();
		auto id = Ludus::Engine::Core::ScriptId { random.NextId() };

		while (HasEditorScriptReference(id))
		{
			id = Ludus::Engine::Core::ScriptId { random.NextId() };
		}

		return id;
	}

	std::vector<std::string> ProjectSession::GetEditorScriptNames() const
	{
		std::vector<std::string> names;
		names.reserve(GetEditorManifest().Scripts.size());

		for (const auto& scriptReference : GetEditorManifest().Scripts)
		{
			names.push_back(scriptReference.Name);
		}

		return names;
	}
}
