#include "pch.h"

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
			.SessionManifest = Ludus::Engine::Runtime::RuntimeManifest::Create(
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

	void ProjectSession::SetActiveScene(Ludus::Engine::Core::SceneHandle sceneHandle)
	{
		if (!GetEditorRuntime().GetSceneRegistry().Contains(sceneHandle))
		{
			throw std::runtime_error("Cannot activate a scene that does not exist in the editor runtime.");
		}

		EditorState.ActiveSceneHandle = sceneHandle;
		EditorState.ActiveSceneState = {
			.IsDirty = false,
			.SavePath = TryGetPersistenceScenePath(sceneHandle)
		};
	}

	void ProjectSession::StartSimulation(Ludus::Engine::Runtime::IHostContext& hostContext)
	{
		if (SimulationRuntime)
		{
			hostContext.AttachRuntime(SimulationRuntime.get());
			return;
		}

		const auto& editorRuntime = *EditorRuntime;
		const auto& persistenceRuntimeManifest = GetPersistenceRuntimeManifest();

		auto simulationRuntime = Ludus::Engine::Runtime::RuntimeInstanceBuilder::Create()
			.UseDefaultPhysics2D()
			.UseDefaultRendering2D()
			.UseDefaultScripting()
			.Build(
				hostContext,
				persistenceRuntimeManifest,
				editorRuntime.GetRuntimeEnvironment()
			);

		for (const auto& sceneReference : persistenceRuntimeManifest.Scenes)
		{
			const auto& scene = editorRuntime.GetSceneRegistry().GetScene(sceneReference.Handle);
			simulationRuntime->GetSceneRegistry().AddScene(Ludus::Engine::Core::Scene::Clone(scene));
		}

		SimulationRuntime = std::move(simulationRuntime);
		SimulationRuntime->Initialize();

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

	const Ludus::Engine::Runtime::RuntimeInstance& ProjectSession::GetPersistenceRuntime() const
	{
		return GetEditorRuntime();
	}

	const Ludus::Engine::Runtime::RuntimeManifest& ProjectSession::GetPersistenceRuntimeManifest() const
	{
		return SessionManifest;
	}

	Ludus::Engine::Runtime::RuntimeManifest& ProjectSession::GetPersistenceRuntimeManifest()
	{
		return SessionManifest;
	}

	const Ludus::Engine::Core::Scene& ProjectSession::GetPersistenceScene(Ludus::Engine::Core::SceneHandle sceneHandle) const
	{
		return GetPersistenceRuntime().GetSceneRegistry().GetScene(sceneHandle);
	}

	std::optional<std::filesystem::path> ProjectSession::TryGetPersistenceScenePath(Ludus::Engine::Core::SceneHandle sceneHandle) const
	{
		std::filesystem::path scenePath;

		for (const auto& sceneReference : GetPersistenceRuntimeManifest().Scenes)
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

	bool ProjectSession::HasPersistenceScriptReference(Ludus::Engine::Components::ScriptHandle handle) const
	{
		for (const auto& scriptReference : GetPersistenceRuntimeManifest().Scripts)
		{
			if (scriptReference.Handle == handle)
			{
				return true;
			}
		}

		return false;
	}

	bool ProjectSession::HasPersistenceScriptReference(std::string_view name) const
	{
		for (const auto& scriptReference : GetPersistenceRuntimeManifest().Scripts)
		{
			if (scriptReference.Name == name)
			{
				return true;
			}
		}

		return false;
	}

	bool ProjectSession::AddOrUpdatePersistenceScriptReference(
		Ludus::Engine::Components::ScriptHandle handle,
		std::string name
	)
	{
		for (auto& scriptReference : GetPersistenceRuntimeManifest().Scripts)
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

		GetPersistenceRuntimeManifest().Scripts.push_back({ handle, std::move(name) });
		return true;
	}

	bool ProjectSession::RemovePersistenceScriptReference(Ludus::Engine::Components::ScriptHandle handle)
	{
		auto& scripts = GetPersistenceRuntimeManifest().Scripts;
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

	Ludus::Engine::Components::ScriptHandle ProjectSession::AllocatePersistenceScriptHandle() const
	{
		auto random = Ludus::Engine::Core::Random();
		auto handle = random.NextId();

		while (HasPersistenceScriptReference(handle))
		{
			handle = random.NextId();
		}

		return handle;
	}

	std::optional<Ludus::Engine::Components::ScriptHandle> ProjectSession::TryFindPersistenceScriptHandleByName(std::string_view name) const
	{
		for (const auto& scriptReference : GetPersistenceRuntimeManifest().Scripts)
		{
			if (scriptReference.Name == name)
			{
				return scriptReference.Handle;
			}
		}

		return std::nullopt;
	}

	std::vector<std::string> ProjectSession::GetPersistenceScriptNames() const
	{
		std::vector<std::string> names;
		names.reserve(GetPersistenceRuntimeManifest().Scripts.size());

		for (const auto& scriptReference : GetPersistenceRuntimeManifest().Scripts)
		{
			names.push_back(scriptReference.Name);
		}

		return names;
	}
}
