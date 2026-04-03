#include "pch.h"

#include <filesystem>
#include <optional>
#include <stdexcept>
#include <utility>

#include <Ludus/Editor/Core/ProjectSession.h>
#include <Ludus/Engine/Core/Random.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Runtime/RuntimeInstanceBuilder.h>

namespace Ludus::Editor::Core
{
	ProjectSession ProjectSession::Create(
		Ludus::Editor::Core::ProjectManifest projectManifest,
		std::unique_ptr<Ludus::Engine::Runtime::RuntimeInstance> editorRuntime,
		Ludus::Engine::Core::SceneHandle activeSceneHandle
	)
	{
		if (!editorRuntime)
		{
			throw std::runtime_error("ProjectSession requires an editor runtime.");
		}

		if (!editorRuntime->GetSceneRegistry().Contains(activeSceneHandle))
		{
			throw std::runtime_error("ProjectSession requires an active scene to exist in the editor runtime.");
		}

		const auto& runtimeManifest = editorRuntime->GetRuntimeManifest();

		std::optional<std::filesystem::path> activeSceneSavePath;
		for (const auto& sceneReference : runtimeManifest.Scenes)
		{
			if (sceneReference.Handle == activeSceneHandle)
			{
				activeSceneSavePath = sceneReference.Path;
				break;
			}
		}

		return {
			.ProjectManifest = std::move(projectManifest),
			.EditorManifest = Ludus::Engine::Runtime::RuntimeManifest::Create(
				runtimeManifest.EntrySceneHandle,
				runtimeManifest.Scenes,
				runtimeManifest.Scripts
			),
			.EditorRuntime = std::move(editorRuntime),
			.SimulationRuntime = nullptr,
			.EditorState = Ludus::Editor::Core::ProjectSessionEditorState::Create(
				activeSceneHandle,
				std::move(activeSceneSavePath)
			)
		};
	}

	void ProjectSession::SetActiveScene(Ludus::Engine::Core::Scene scene, std::optional<std::filesystem::path> sceneSavePath)
	{
		auto& sceneRegistry = GetEditorRuntime().GetSceneRegistry();
		sceneRegistry.Clear();

		const auto sceneHandle = scene.Handle;
		sceneRegistry.AddScene(std::move(scene));

		EditorState.ActiveSceneHandle = sceneHandle;
		EditorState.ActiveSceneState = {
			.IsDirty = false,
			.SavePath = sceneSavePath ? sceneSavePath : TryGetEditorScenePath(sceneHandle)
		};
		GetEditorRuntime().GetScenePresentationState().CurrentSceneHandle = sceneHandle;
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
		const auto activeSceneHandle = EditorState.ActiveSceneHandle;
		const auto& activeScene = editorRuntime.GetSceneRegistry().GetScene(activeSceneHandle);

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

	Ludus::Engine::Core::Scene& ProjectSession::GetEditorScene(Ludus::Engine::Core::SceneHandle sceneHandle)
	{
		return GetEditorRuntime().GetSceneRegistry().GetScene(sceneHandle);
	}

	const Ludus::Engine::Core::Scene& ProjectSession::GetEditorScene(Ludus::Engine::Core::SceneHandle sceneHandle) const
	{
		return GetEditorRuntime().GetSceneRegistry().GetScene(sceneHandle);
	}

	std::optional<std::filesystem::path> ProjectSession::TryGetEditorScenePath(Ludus::Engine::Core::SceneHandle sceneHandle) const
	{
		std::filesystem::path scenePath;

		for (const auto& sceneReference : GetEditorManifest().Scenes)
		{
			if (sceneReference.Handle == sceneHandle)
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
		Ludus::Engine::Core::SceneHandle handle,
		std::string name,
		std::filesystem::path path
	)
	{
		for (auto& sceneReference : GetEditorManifest().Scenes)
		{
			if (sceneReference.Handle == handle)
			{
				sceneReference.Name = std::move(name);
				sceneReference.Path = std::move(path);

				return;
			}
		}

		GetEditorManifest().Scenes.push_back({ handle, std::move(name), std::move(path) });
	}

	bool ProjectSession::HasEditorScriptReference(Ludus::Engine::Components::ScriptHandle handle) const
	{
		for (const auto& scriptReference : GetEditorManifest().Scripts)
		{
			if (scriptReference.Handle == handle)
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
		Ludus::Engine::Components::ScriptHandle handle,
		std::string name
	)
	{
		for (auto& scriptReference : GetEditorManifest().Scripts)
		{
			if (scriptReference.Handle == handle)
			{
				if (scriptReference.Name == name)
				{
					return false;
				}

				scriptReference.Name = std::move(name);
				return true;
			}
		}

		GetEditorManifest().Scripts.push_back({ handle, std::move(name) });
		return true;
	}

	bool ProjectSession::RemoveEditorScriptReference(Ludus::Engine::Components::ScriptHandle handle)
	{
		auto& scripts = GetEditorManifest().Scripts;
		for (auto iter = scripts.begin(); iter != scripts.end(); ++iter)
		{
			if (iter->Handle == handle)
			{
				scripts.erase(iter);
				return true;
			}
		}

		return false;
	}

	Ludus::Engine::Components::ScriptHandle ProjectSession::AllocateEditorScriptHandle() const
	{
		auto random = Ludus::Engine::Core::Random();
		auto handle = random.NextId();

		while (HasEditorScriptReference(handle))
		{
			handle = random.NextId();
		}

		return handle;
	}

	std::optional<Ludus::Engine::Components::ScriptHandle> ProjectSession::TryFindEditorScriptHandleByName(std::string_view name) const
	{
		for (const auto& scriptReference : GetEditorManifest().Scripts)
		{
			if (scriptReference.Name == name)
			{
				return scriptReference.Handle;
			}
		}

		return std::nullopt;
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
