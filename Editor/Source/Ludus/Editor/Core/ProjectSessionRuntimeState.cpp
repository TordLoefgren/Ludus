#include "pch.h"

#include <memory>
#include <utility>

#include <Ludus/Editor/Core/ProjectSessionRuntimeState.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Runtime/RuntimeInstance.h>
#include <Ludus/Engine/Runtime/RuntimeInstanceBuilder.h>

namespace Ludus::Editor::Core
{
	ProjectSessionRuntimeState::ProjectSessionRuntimeState(
		std::unique_ptr<Ludus::Engine::Runtime::RuntimeInstance> editorRuntime
	) :
		m_EditorRuntime(std::move(editorRuntime)),
		m_SimulationRuntime(nullptr)
	{ }

	Ludus::Engine::Runtime::RuntimeInstance& ProjectSessionRuntimeState::GetActiveRuntime()
	{
		return m_SimulationRuntime != nullptr ? *m_SimulationRuntime : *m_EditorRuntime;
	}

	const Ludus::Engine::Runtime::RuntimeInstance& ProjectSessionRuntimeState::GetActiveRuntime() const
	{
		return m_SimulationRuntime != nullptr ? *m_SimulationRuntime : *m_EditorRuntime;
	}

	ProjectSessionRuntimeState ProjectSessionRuntimeState::Create(
		std::unique_ptr<Ludus::Engine::Runtime::RuntimeInstance> editorRuntime
	)
	{
		return { std::move(editorRuntime) };
	}

	void ProjectSessionRuntimeState::StartSimulation(
		Ludus::Engine::Runtime::IHostContext& hostContext,
		Ludus::Engine::Runtime::RuntimeManifest runtimeManifest,
		const Ludus::Engine::Core::SceneId activeSceneId
	)
	{
		if (m_SimulationRuntime)
		{
			hostContext.AttachRuntime(m_SimulationRuntime.get());
			return;
		}

		const auto& editorRuntime = *m_EditorRuntime;
		const auto& activeScene = editorRuntime.GetSceneRegistry().GetScene(activeSceneId);

		auto simulationRuntime = Ludus::Engine::Runtime::RuntimeInstanceBuilder::Create()
			.UseDefaultPhysics2D()
			.UseDefaultRendering2D()
			.UseDefaultSceneManagement()
			.UseDefaultScripting()
			.WithRenderPresentationSettings(editorRuntime.GetRenderPresentationSettings())
			.WithRuntimeManifest(runtimeManifest)
			.WithRuntimeEnvironment(editorRuntime.GetRuntimeEnvironment())
			.WithExplicitScene(Ludus::Engine::Core::Scene::Clone(activeScene))
			.Build(hostContext);

		m_SimulationRuntime = std::move(simulationRuntime);

		hostContext.AttachRuntime(m_SimulationRuntime.get());
	}

	void ProjectSessionRuntimeState::PauseSimulation(Ludus::Engine::Runtime::IHostContext& hostContext)
	{
		if (!m_SimulationRuntime)
		{
			return;
		}

		hostContext.AttachRuntime(m_SimulationRuntime.get());
	}

	void ProjectSessionRuntimeState::StopSimulation(Ludus::Engine::Runtime::IHostContext& hostContext)
	{
		if (m_SimulationRuntime)
		{
			hostContext.AttachRuntime(m_EditorRuntime.get());
			m_SimulationRuntime->Shutdown();
			m_SimulationRuntime.reset();

			return;
		}

		hostContext.AttachRuntime(m_EditorRuntime.get());
	}

	void ProjectSessionRuntimeState::ShutdownRuntimes()
	{
		if (m_SimulationRuntime)
		{
			m_SimulationRuntime->Shutdown();
			m_SimulationRuntime.reset();
		}

		if (m_EditorRuntime)
		{
			m_EditorRuntime->Shutdown();
		}
	}

	void ProjectSessionRuntimeState::AttachEditorRuntime(Ludus::Engine::Runtime::IHostContext& hostContext)
	{
		hostContext.AttachRuntime(m_EditorRuntime.get());
	}

	void ProjectSessionRuntimeState::ApplyRuntimeLaunchSettings(const Ludus::Engine::Runtime::RuntimeLaunchSettings& settings)
	{
		auto& renderPresentationSettings = m_EditorRuntime->GetRenderPresentationSettings();
		renderPresentationSettings.InternalResolution = settings.PresentationSettings.InternalResolution;
		renderPresentationSettings.UseFixedRenderResolution = settings.PresentationSettings.UseFixedRenderResolution;
	}

	Ludus::Engine::Core::Scene& ProjectSessionRuntimeState::GetEditorScene(Ludus::Engine::Core::SceneId id) const
	{
		return m_EditorRuntime->GetSceneRegistry().GetScene(id);
	}
}
