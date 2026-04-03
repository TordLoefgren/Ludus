#include "pch.h"

#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include <Ludus/Engine/Core/ExecutionFlags.h>
#include <Ludus/Engine/Core/Mask.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Graphics/MainRenderViewSystem.h>
#include <Ludus/Engine/Graphics/RenderingSystem2D.h>
#include <Ludus/Engine/Graphics/RenderViewSystem.h>
#include <Ludus/Engine/Physics/Core/PhysicsSystem2D.h>
#include <Ludus/Engine/Runtime/ISystem.h>
#include <Ludus/Engine/Runtime/RuntimeInstance.h>
#include <Ludus/Engine/Runtime/RuntimeInstanceBuilder.h>
#include <Ludus/Engine/Runtime/SystemConstraints.h>
#include <Ludus/Engine/Runtime/SystemDescriptor.h>
#include <Ludus/Engine/Runtime/SystemPhase.h>
#include <Ludus/Engine/Runtime/SystemPhaseOrder.h>
#include <Ludus/Engine/Scripting/ScriptSystem.h>

namespace Ludus::Engine::Runtime
{
	std::unique_ptr<Ludus::Engine::Runtime::RuntimeInstance> RuntimeInstanceBuilder::Build(IHostContext& hostContext)
	{
		LUDUS_ASSERT(
			m_RuntimeEnvironment && m_RuntimeManifest && m_InitialScene,
			"Cannot build a runtime instance without runtime environment, runtime manifest, and an initial scene."
		);

		auto runtime = RuntimeInstance::Create(
			hostContext,
			std::move(*m_RuntimeManifest),
			std::move(*m_RuntimeEnvironment),
			std::move(*m_InitialScene),
			m_InitialSceneMode,
			std::move(m_RenderingConfiguration),
			std::move(m_RenderPresentationSettings),
			std::move(m_PhysicsConfiguration)
		);

		if (m_UseDefaultMainRenderView)
		{
			runtime->AddSystem(
				Ludus::Engine::Runtime::SystemDescriptor { SystemPhase::Update },
				std::make_unique<Ludus::Engine::Graphics::MainRenderViewSystem>(
					runtime->GetHostContext(),
					runtime->GetRenderViewRequestRegistry(),
					runtime->GetScenePresentationState()
				)
			);
		}

		if (m_UseDefaultPhysics2D)
		{
			auto& physicsConfiguration = runtime->GetPhysicsConfiguration();
			auto physicsSystem = std::make_unique<Ludus::Engine::Physics::Core::PhysicsSystem2D>(physicsConfiguration, runtime->GetSceneRegistry());
			auto constraints = Ludus::Engine::Runtime::SystemConstraints::Create()
				.RequireAllOf(Ludus::Engine::Core::Mask(Ludus::Engine::Core::ExecutionFlags::PhysicsEnabled) | Ludus::Engine::Core::Mask(Ludus::Engine::Core::ExecutionFlags::SimulationEnabled));

			runtime->AddSystem(
				Ludus::Engine::Runtime::SystemDescriptor { SystemPhase::FixedUpdate, SystemPhaseOrder::Before, constraints },
				std::move(physicsSystem)
			);
		}

		if (m_UseDefaultRendering2D)
		{
			auto renderViewSystem = std::make_unique<Ludus::Engine::Graphics::RenderViewSystem>(
				runtime->GetHostContext(),
				m_RenderViewConfiguration,
				runtime->GetRenderViewRegistry(),
				runtime->GetRenderViewRequestRegistry(),
				runtime->GetSceneRegistry()
			);
			auto constraints = Ludus::Engine::Runtime::SystemConstraints::Create()
				.RequireAnyOf(Ludus::Engine::Core::Mask(Ludus::Engine::Core::ExecutionFlags::RenderingEnabled));

			runtime->AddSystem(
				{
					Ludus::Engine::Runtime::SystemDescriptor { SystemPhase::BeginFrame, SystemPhaseOrder::Before },
					Ludus::Engine::Runtime::SystemDescriptor { SystemPhase::Render, SystemPhaseOrder::Before, constraints }
				},
				std::move(renderViewSystem)
			);

			auto renderingSystem = std::make_unique<Ludus::Engine::Graphics::RenderingSystem2D>(
				m_RenderingOptions,
				runtime->GetRenderingConfiguration(),
				runtime->GetRenderViewRegistry(),
				runtime->GetSceneRegistry(),
				runtime->GetRuntimeEnvironment()
			);

			runtime->AddSystem(
				Ludus::Engine::Runtime::SystemDescriptor { SystemPhase::Render, SystemPhaseOrder::Before },
				std::move(renderingSystem)
			);
		}

		if (m_UseDefaultScripting)
		{
			auto constraints = Ludus::Engine::Runtime::SystemConstraints::Create()
				.RequireAllOf(
					Ludus::Engine::Core::Mask(Ludus::Engine::Core::ExecutionFlags::ScriptingEnabled) |
					Ludus::Engine::Core::Mask(Ludus::Engine::Core::ExecutionFlags::SimulationSessionEnabled)
				);

			auto scriptingSystem = std::make_unique<Ludus::Engine::Scripting::ScriptSystem>(
				runtime->GetHostContext(),
				runtime->GetSceneRegistry(),
				runtime->GetScenePresentationState().CurrentSceneHandle,
				runtime->GetPhysicsConfiguration().QueryCache.get(),
				runtime->GetRuntimeEnvironment().ScriptModulePath
			);

			runtime->AddSystem(
				Ludus::Engine::Runtime::SystemDescriptor { SystemPhase::Update, SystemPhaseOrder::Before, constraints },
				std::move(scriptingSystem)
			);
		}

		for (auto& command : m_BuilderCommands)
		{
			command(*runtime);
		}

		return runtime;
	}

	RuntimeInstanceBuilder& RuntimeInstanceBuilder::Configure(RuntimeInstanceBuilderCommand command)
	{
		m_BuilderCommands.emplace_back(std::move(command));
		return *this;
	}

	RuntimeInstanceBuilder& RuntimeInstanceBuilder::WithRenderingOptions(Ludus::Engine::Graphics::RenderingOptions renderingOptions)
	{
		m_RenderingOptions = std::move(renderingOptions);
		return *this;
	}

	RuntimeInstanceBuilder& RuntimeInstanceBuilder::WithRenderingConfiguration(Ludus::Engine::Graphics::RenderingConfiguration2D renderingConfiguration)
	{
		m_RenderingConfiguration = std::move(renderingConfiguration);
		return *this;
	}

	RuntimeInstanceBuilder& RuntimeInstanceBuilder::WithRenderPresentationSettings(Ludus::Engine::Graphics::RenderPresentationSettings renderPresentationSettings)
	{
		m_RenderPresentationSettings = std::move(renderPresentationSettings);
		return *this;
	}

	RuntimeInstanceBuilder& RuntimeInstanceBuilder::WithRenderViewConfiguration(Ludus::Engine::Graphics::RenderViewConfiguration renderViewConfiguration)
	{
		m_RenderViewConfiguration = std::move(renderViewConfiguration);
		return *this;
	}

	RuntimeInstanceBuilder& RuntimeInstanceBuilder::WithPhysicsConfiguration(Ludus::Engine::Physics::Core::PhysicsConfiguration2D physicsConfiguration)
	{
		m_PhysicsConfiguration = std::move(physicsConfiguration);
		return *this;
	}

	RuntimeInstanceBuilder& RuntimeInstanceBuilder::WithRuntimeEnvironment(RuntimeEnvironment runtimeEnvironment)
	{
		m_RuntimeEnvironment = std::move(runtimeEnvironment);

		return *this;
	}

	RuntimeInstanceBuilder& RuntimeInstanceBuilder::WithRuntimeManifest(RuntimeManifest manifest)
	{
		m_RuntimeManifest = std::move(manifest);

		return *this;
	}

	RuntimeInstanceBuilder& RuntimeInstanceBuilder::WithEntryScene(Ludus::Engine::Core::Scene entryScene)
	{
		m_InitialScene = std::move(entryScene);
		m_InitialSceneMode = InitialSceneMode::Entry;

		return *this;
	}

	RuntimeInstanceBuilder& RuntimeInstanceBuilder::WithExplicitScene(Ludus::Engine::Core::Scene explicitScene)
	{
		m_InitialScene = std::move(explicitScene);
		m_InitialSceneMode = InitialSceneMode::Explicit;

		return *this;
	}

	RuntimeInstanceBuilder& RuntimeInstanceBuilder::UseDefaultPhysics2D()
	{
		if (m_UseDefaultPhysics2D)
		{
			LUDUS_LOG_WARN("Invalid operation: Cannot add default physics system more than once.");
		}

		m_UseDefaultPhysics2D = true;

		return *this;
	}

	RuntimeInstanceBuilder& RuntimeInstanceBuilder::UseDefaultMainRenderView()
	{
		if (m_UseDefaultMainRenderView)
		{
			LUDUS_LOG_WARN("Invalid operation: Cannot add default main render view system more than once.");
		}

		m_UseDefaultMainRenderView = true;

		return *this;
	}

	RuntimeInstanceBuilder& RuntimeInstanceBuilder::UseDefaultRendering2D()
	{
		if (m_UseDefaultRendering2D)
		{
			LUDUS_LOG_WARN("Invalid operation: Cannot add default rendering system more than once.");
		}

		m_UseDefaultRendering2D = true;

		return *this;
	}

	RuntimeInstanceBuilder& RuntimeInstanceBuilder::UseDefaultScripting()
	{
		if (m_UseDefaultScripting)
		{
			LUDUS_LOG_WARN("Invalid operation: Cannot add default scripting system more than once.");
		}

		m_UseDefaultScripting = true;

		return *this;
	}
}
