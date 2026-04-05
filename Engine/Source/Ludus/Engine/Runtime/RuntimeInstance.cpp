#include "pch.h"

#include <memory>
#include <utility>

#include <Ludus/Engine/Core/RenderViewRegistry.h>
#include <Ludus/Engine/Core/RenderViewRequestRegistry.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Graphics/RenderingConfiguration2D.h>
#include <Ludus/Engine/Graphics/RenderPresentationSettings.h>
#include <Ludus/Engine/Physics/Core/PhysicsConfiguration2D.h>
#include <Ludus/Engine/Runtime/IHostContext.h>
#include <Ludus/Engine/Runtime/InitialSceneMode.h>
#include <Ludus/Engine/Runtime/ISystem.h>
#include <Ludus/Engine/Runtime/RuntimeEnvironment.h>
#include <Ludus/Engine/Runtime/RuntimeInstance.h>
#include <Ludus/Engine/Runtime/SystemScheduler.h>

namespace Ludus::Engine::Runtime
{
	RuntimeInstance::RuntimeInstance(
		IHostContext& hostContext,
		RuntimeManifest runtimeManifest,
		RuntimeEnvironment runtimeEnvironment,
		Ludus::Engine::Core::Scene initialScene,
		InitialSceneMode initialSceneMode,
		Ludus::Engine::Graphics::RenderingConfiguration2D renderingConfiguration,
		Ludus::Engine::Graphics::RenderPresentationSettings renderPresentationSettings,
		Ludus::Engine::Physics::Core::PhysicsConfiguration2D physicsConfiguration
	) :
		m_RuntimeManifest(std::move(runtimeManifest)),
		m_RuntimeEnvironment(std::move(runtimeEnvironment)),
		m_HostContext(hostContext),
		m_RenderPresentationSettings(std::move(renderPresentationSettings)),
		m_RenderingConfiguration(std::move(renderingConfiguration)),
		m_PhysicsConfiguration(std::move(physicsConfiguration)),
		m_RenderViewRegistry(),
		m_RenderViewRequestRegistry(),
		m_SceneRegistry(),
		m_ScenePresentationState(),
		m_Scheduler()
	{
		if (initialSceneMode == InitialSceneMode::Entry)
		{
			LUDUS_ASSERT(
				m_RuntimeManifest.EntrySceneId == initialScene.Id,
				"The initial scene must respect the runtime manifest when InitialSceneMode is Entry."
			);
		}

		m_ScenePresentationState.CurrentSceneId = m_SceneRegistry.AddScene(std::move(initialScene));
	}

	RuntimeInstance::~RuntimeInstance() = default;

	std::unique_ptr<RuntimeInstance> RuntimeInstance::Create(
		IHostContext& hostContext,
		RuntimeManifest RuntimeManifest,
		RuntimeEnvironment runtimeEnvironment,
		Ludus::Engine::Core::Scene initialScene,
		InitialSceneMode initialSceneMode,
		Ludus::Engine::Graphics::RenderingConfiguration2D renderingConfiguration,
		Ludus::Engine::Graphics::RenderPresentationSettings renderPresentationSettings,
		Ludus::Engine::Physics::Core::PhysicsConfiguration2D physicsConfiguration
	)
	{
		auto runtime = std::unique_ptr<RuntimeInstance>(new RuntimeInstance(
			hostContext,
			std::move(RuntimeManifest),
			std::move(runtimeEnvironment),
			std::move(initialScene),
			initialSceneMode,
			std::move(renderingConfiguration),
			std::move(renderPresentationSettings),
			std::move(physicsConfiguration)
		));

		return runtime;
	}

	void RuntimeInstance::AddSystem(SystemDescriptor info, std::unique_ptr<ISystem> system)
	{
		m_Scheduler.AddSystem(std::move(info), std::move(system));
	}

	void RuntimeInstance::AddSystem(std::initializer_list<SystemDescriptor> info, std::unique_ptr<ISystem> system)
	{
		m_Scheduler.AddSystem(std::move(info), std::move(system));
	}

	void RuntimeInstance::Initialize()
	{
		LUDUS_ASSERT(
			m_SceneRegistry.Contains(m_ScenePresentationState.CurrentSceneId),
			"The current scene was not found in the scene registry."
		);

		m_Scheduler.AttachSystems();
	}

	void RuntimeInstance::Shutdown()
	{
		m_Scheduler.DetachSystems();
	}

	void RuntimeInstance::UpdateTransitions(const Ludus::Engine::Core::FlagSet& executionFlags)
	{
		m_Scheduler.UpdateTransitions(executionFlags);
	}

	void RuntimeInstance::Run(SystemPhase phase, Ludus::Engine::Core::FlagSet& executionFlags, float time)
	{
		m_Scheduler.Run(phase, executionFlags, time);
	}
}
