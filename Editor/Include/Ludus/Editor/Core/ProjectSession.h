#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <Ludus/Editor/Core/ProjectManifest.h>
#include <Ludus/Editor/Core/SceneSessionState.h>
#include <Ludus/Editor/Core/SelectionManager.h>
#include <Ludus/Engine/Core/RenderViewRegistry.h>
#include <Ludus/Engine/Core/RenderViewRequestRegistry.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Graphics/RenderPresentationSettings.h>
#include <Ludus/Engine/Runtime/IHostContext.h>
#include <Ludus/Engine/Runtime/RuntimeInstance.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::Editor::Core
{
	struct ProjectSessionEditorState
	{
		SceneSessionState ActiveSceneState;
		Ludus::Engine::Core::SceneHandle ActiveSceneHandle;
		Ludus::Editor::Core::SelectionManager Selection;

		static ProjectSessionEditorState Create(
			Ludus::Engine::Core::SceneHandle activeSceneHandle,
			std::optional<std::filesystem::path> activeSceneSavePath = std::nullopt
		)
		{
			return {
				.ActiveSceneState = { .IsDirty = false, .SavePath = std::move(activeSceneSavePath) },
				.ActiveSceneHandle = activeSceneHandle,
				.Selection = { }
			};
		}
	};

	struct ProjectSession
	{
		Ludus::Editor::Core::ProjectManifest ProjectManifest;
		Ludus::Engine::Runtime::RuntimeManifest EditorManifest;
		std::unique_ptr<Ludus::Engine::Runtime::RuntimeInstance> EditorRuntime;
		std::unique_ptr<Ludus::Engine::Runtime::RuntimeInstance> SimulationRuntime;
		ProjectSessionEditorState EditorState;

		static ProjectSession Create(
			Ludus::Editor::Core::ProjectManifest projectManifest,
			std::unique_ptr<Ludus::Engine::Runtime::RuntimeInstance> editorRuntime,
			Ludus::Engine::Core::SceneHandle activeSceneHandle
		);

#pragma region Editor Helpers

		void SetActiveScene(
			Ludus::Engine::Core::Scene scene,
			std::optional<std::filesystem::path> sceneSavePath = std::nullopt
		);

		void SetActiveSceneSavePath(std::filesystem::path sceneSavePath)
		{
			EditorState.ActiveSceneState.SavePath = sceneSavePath;
		}

		bool ActiveSceneHasSavePath() const
		{
			return EditorState.ActiveSceneState.SavePath.has_value();
		}

		void MarkSceneDirty()
		{
			EditorState.ActiveSceneState.IsDirty = true;
		}

		void ClearSceneDirty()
		{
			EditorState.ActiveSceneState.IsDirty = false;
		}

		bool IsSceneDirty() const
		{
			return EditorState.ActiveSceneState.IsDirty;
		}

		bool IsProjectDirty() const
		{
			return IsSceneDirty();
		}

		void StartSimulation(Ludus::Engine::Runtime::IHostContext& hostContext);
		void PauseSimulation(Ludus::Engine::Runtime::IHostContext& hostContext);
		void StopSimulation(Ludus::Engine::Runtime::IHostContext& hostContext);

		void ShutdownRuntimes();

#pragma endregion

#pragma region Editor State Helpers

		const Ludus::Engine::Runtime::RuntimeManifest& GetEditorManifest() const;

		Ludus::Engine::Runtime::RuntimeManifest& GetEditorManifest();

		Ludus::Engine::Core::Scene& GetEditorScene(Ludus::Engine::Core::SceneHandle sceneHandle);
		const Ludus::Engine::Core::Scene& GetEditorScene(Ludus::Engine::Core::SceneHandle sceneHandle) const;

		std::optional<std::filesystem::path> TryGetEditorScenePath(Ludus::Engine::Core::SceneHandle sceneHandle) const;
		void AddOrUpdateEditorSceneReference(
			Ludus::Engine::Core::SceneHandle handle,
			std::string name,
			std::filesystem::path path
		);

		bool HasEditorScriptReference(Ludus::Engine::Components::ScriptHandle handle) const;

		bool HasEditorScriptReference(std::string_view name) const;

		bool AddOrUpdateEditorScriptReference(
			Ludus::Engine::Components::ScriptHandle handle,
			std::string name
		);

		bool RemoveEditorScriptReference(Ludus::Engine::Components::ScriptHandle handle);
		Ludus::Engine::Components::ScriptHandle AllocateEditorScriptHandle() const;
		std::optional<Ludus::Engine::Components::ScriptHandle> TryFindEditorScriptHandleByName(std::string_view name) const;
		std::vector<std::string> GetEditorScriptNames() const;

#pragma endregion

#pragma region Getters

		Ludus::Engine::Runtime::RuntimeInstance& GetActiveRuntime()
		{
			if (SimulationRuntime)
			{
				return *SimulationRuntime;
			}

			return *EditorRuntime;
		}

		const Ludus::Engine::Runtime::RuntimeInstance& GetActiveRuntime() const
		{
			if (SimulationRuntime)
			{
				return *SimulationRuntime;
			}

			return *EditorRuntime;
		}

		Ludus::Engine::Runtime::RuntimeInstance& GetEditorRuntime() { return *EditorRuntime; }
		const Ludus::Engine::Runtime::RuntimeInstance& GetEditorRuntime() const { return *EditorRuntime; }

		Ludus::Engine::Runtime::RuntimeInstance& GetSimulationRuntime() { return *SimulationRuntime; }
		const Ludus::Engine::Runtime::RuntimeInstance& GetSimulationRuntime() const { return *SimulationRuntime; }

		bool IsSimulating() const { return SimulationRuntime != nullptr; }

		const Ludus::Engine::Runtime::RuntimeEnvironment& GetRuntimeEnvironment() const
		{
			return GetActiveRuntime().GetRuntimeEnvironment();
		}

		Ludus::Engine::Core::SceneRegistry& GetSceneRegistry() { return GetActiveRuntime().GetSceneRegistry(); }
		const Ludus::Engine::Core::SceneRegistry& GetSceneRegistry() const { return GetActiveRuntime().GetSceneRegistry(); }

		Ludus::Engine::Core::RenderViewRegistry& GetRenderViewRegistry() { return GetActiveRuntime().GetRenderViewRegistry(); }
		const Ludus::Engine::Core::RenderViewRegistry& GetRenderViewRegistry() const { return GetActiveRuntime().GetRenderViewRegistry(); }

		Ludus::Engine::Graphics::RenderPresentationSettings& GetRenderPresentationSettings() { return GetActiveRuntime().GetRenderPresentationSettings(); }
		const Ludus::Engine::Graphics::RenderPresentationSettings& GetRenderPresentationSettings() const { return GetActiveRuntime().GetRenderPresentationSettings(); }

		Ludus::Engine::Core::RenderViewRequestRegistry& GetRenderViewRequestRegistry() { return GetActiveRuntime().GetRenderViewRequestRegistry(); }
		const Ludus::Engine::Core::RenderViewRequestRegistry& GetRenderViewRequestRegistry() const { return GetActiveRuntime().GetRenderViewRequestRegistry(); }

		const std::filesystem::path& GetProjectRoot() const { return ProjectManifest.ProjectRoot; }

#pragma endregion

	};
}
