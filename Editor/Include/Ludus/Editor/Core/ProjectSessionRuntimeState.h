#pragma once

#include <memory>

#include <Ludus/Engine/Core/AssetRegistry.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Core/RenderViewRequestRegistry.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Graphics/RenderPresentationSettings.h>
#include <Ludus/Engine/Runtime/IHostContext.h>
#include <Ludus/Engine/Runtime/RuntimeInstance.h>
#include <Ludus/Engine/Runtime/RuntimeLaunchSettings.h>
#include <Ludus/Engine/Runtime/SceneRuntimeState.h>

namespace Ludus::Editor::Core
{
	class ProjectSessionRuntimeState
	{
	private:
		std::unique_ptr<Ludus::Engine::Runtime::RuntimeInstance> m_EditorRuntime;
		std::unique_ptr<Ludus::Engine::Runtime::RuntimeInstance> m_SimulationRuntime;

		ProjectSessionRuntimeState(
			std::unique_ptr<Ludus::Engine::Runtime::RuntimeInstance> editorRuntime
		);

		Ludus::Engine::Runtime::RuntimeInstance& GetActiveRuntime();
		const Ludus::Engine::Runtime::RuntimeInstance& GetActiveRuntime() const;

	public:
		static ProjectSessionRuntimeState Create(
			std::unique_ptr<Ludus::Engine::Runtime::RuntimeInstance> editorRuntime
		);

		void StartSimulation(
			Ludus::Engine::Runtime::IHostContext& hostContext,
			Ludus::Engine::Runtime::RuntimeManifest runtimeManifest,
			const Ludus::Engine::Core::SceneId activeSceneId
		);
		void PauseSimulation(Ludus::Engine::Runtime::IHostContext& hostContext);
		void StopSimulation(Ludus::Engine::Runtime::IHostContext& hostContext);

		void ShutdownRuntimes();
		void AttachEditorRuntime(Ludus::Engine::Runtime::IHostContext& hostContext);
		void ApplyRuntimeLaunchSettings(const Ludus::Engine::Runtime::RuntimeLaunchSettings& settings);

		bool IsSimulationActive() const { return m_SimulationRuntime != nullptr; }

		Ludus::Engine::Core::Scene& GetEditorScene(Ludus::Engine::Core::SceneId id) const;
		Ludus::Engine::Core::AssetRegistry& GetEditorAssetRegistry() { return m_EditorRuntime->GetAssetRegistry(); }
		const Ludus::Engine::Core::AssetRegistry& GetEditorAssetRegistry() const { return m_EditorRuntime->GetAssetRegistry(); }

		Ludus::Engine::Core::SceneRegistry& GetEditorSceneRegistry() { return m_EditorRuntime->GetSceneRegistry(); }
		const Ludus::Engine::Core::SceneRegistry& GetEditorSceneRegistry() const { return m_EditorRuntime->GetSceneRegistry(); }
		Ludus::Engine::Core::SceneRegistry& GetActiveSceneRegistry() { return GetActiveRuntime().GetSceneRegistry(); }
		const Ludus::Engine::Core::SceneRegistry& GetActiveSceneRegistry() const { return GetActiveRuntime().GetSceneRegistry(); }

		Ludus::Engine::Graphics::RenderPresentationSettings& GetEditorRenderPresentationSettings() { return m_EditorRuntime->GetRenderPresentationSettings(); }
		const Ludus::Engine::Graphics::RenderPresentationSettings& GetEditorRenderPresentationSettings() const { return m_EditorRuntime->GetRenderPresentationSettings(); }
		Ludus::Engine::Graphics::RenderPresentationSettings& GetActiveRenderPresentationSettings() { return GetActiveRuntime().GetRenderPresentationSettings(); }
		const Ludus::Engine::Graphics::RenderPresentationSettings& GetActiveRenderPresentationSettings() const { return GetActiveRuntime().GetRenderPresentationSettings(); }

		Ludus::Engine::Core::RenderViewRequestRegistry& GetEditorRenderViewRequestRegistry() { return m_EditorRuntime->GetRenderViewRequestRegistry(); }
		const Ludus::Engine::Core::RenderViewRequestRegistry& GetEditorRenderViewRequestRegistry() const { return m_EditorRuntime->GetRenderViewRequestRegistry(); }
		Ludus::Engine::Core::RenderViewRequestRegistry& GetActiveRenderViewRequestRegistry() { return GetActiveRuntime().GetRenderViewRequestRegistry(); }
		const Ludus::Engine::Core::RenderViewRequestRegistry& GetActiveRenderViewRequestRegistry() const { return GetActiveRuntime().GetRenderViewRequestRegistry(); }

		Ludus::Engine::Runtime::SceneRuntimeState& GetEditorSceneRuntimeState() { return m_EditorRuntime->GetSceneRuntimeState(); }
		const Ludus::Engine::Runtime::SceneRuntimeState& GetEditorSceneRuntimeState() const { return m_EditorRuntime->GetSceneRuntimeState(); }
		Ludus::Engine::Runtime::SceneRuntimeState& GetActiveSceneRuntimeState() { return GetActiveRuntime().GetSceneRuntimeState(); }
		const Ludus::Engine::Runtime::SceneRuntimeState& GetActiveSceneRuntimeState() const { return GetActiveRuntime().GetSceneRuntimeState(); }
	};
}
