#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <Ludus/Editor/Core/ProjectManifest.h>
#include <Ludus/Editor/Core/ProjectSessionEditorState.h>
#include <Ludus/Editor/Core/ProjectSessionPersistence.h>
#include <Ludus/Editor/Core/ProjectSessionRuntimeState.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Runtime/IHostContext.h>
#include <Ludus/Engine/Runtime/RuntimeInstance.h>
#include <Ludus/Engine/Runtime/RuntimeLaunchSettings.h>

namespace Ludus::Editor::Core
{
	struct ProjectSession
	{
		ProjectSessionPersistence Persistence;
		ProjectSessionEditorState EditorState;
		ProjectSessionRuntimeState RuntimeState;

		static ProjectSession Create(
			ProjectManifest projectManifest,
			std::unique_ptr<Ludus::Engine::Runtime::RuntimeInstance> editorRuntime,
			Ludus::Engine::Runtime::RuntimeLaunchSettings runtimeLaunchSettings,
			Ludus::Engine::Core::SceneId activeSceneId
		);

		void SetActiveScene(
			Ludus::Engine::Core::Scene scene,
			std::optional<std::filesystem::path> sceneSavePath = std::nullopt
		);
		Ludus::Engine::Core::SceneId GetPresentedSceneId() const;

		void StartSimulation(Ludus::Engine::Runtime::IHostContext& hostContext);
		void PauseSimulation(Ludus::Engine::Runtime::IHostContext& hostContext);
		void StopSimulation(Ludus::Engine::Runtime::IHostContext& hostContext);

		void ShutdownRuntimes();

		bool UpdateRuntimeLaunchSettings(
			const Ludus::Engine::Runtime::RuntimeLaunchSettings& runtimeLaunchSettings
		);

		bool AddOrUpdateAssetReference(
			Ludus::Engine::Core::AssetId id,
			Ludus::Engine::Core::AssetType type,
			std::filesystem::path path
		);

		bool AddOrUpdateSceneReference(
			Ludus::Engine::Core::SceneId id,
			std::string name,
			std::filesystem::path path
		);

		bool AddOrUpdateScriptReference(
			Ludus::Engine::Core::ScriptId id,
			std::string name
		);

		bool RemoveScriptReference(Ludus::Engine::Core::ScriptId id);

		void MarkActiveSceneDirty();
	};
}
