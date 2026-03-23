#pragma once

#include <initializer_list>
#include <memory>
#include <utility>

#include <Ludus/Engine/Core/FlagSet.h>
#include <Ludus/Engine/Core/RenderViewRegistry.h>
#include <Ludus/Engine/Core/RenderViewRequestRegistry.h>
#include <Ludus/Engine/Graphics/RenderingConfiguration2D.h>
#include <Ludus/Engine/Graphics/RenderPresentationSettings.h>
#include <Ludus/Engine/Physics/Core/PhysicsConfiguration2D.h>
#include <Ludus/Engine/Runtime/IHostContext.h>
#include <Ludus/Engine/Runtime/RuntimeEnvironment.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>
#include <Ludus/Engine/Runtime/SystemPhase.h>
#include <Ludus/Engine/Runtime/SystemScheduler.h>

namespace Ludus::Engine::Runtime
{
	class RuntimeInstance
	{
	private:
		const RuntimeManifest m_RuntimeManifest;
		const RuntimeEnvironment m_RuntimeEnvironment;
		IHostContext& m_HostContext;

		Ludus::Engine::Graphics::RenderPresentationSettings m_RenderPresentationSettings;
		Ludus::Engine::Graphics::RenderingConfiguration2D m_RenderingConfiguration;
		Ludus::Engine::Physics::Core::PhysicsConfiguration2D m_PhysicsConfiguration;
		Ludus::Engine::Core::RenderViewRegistry m_RenderViewRegistry;
		Ludus::Engine::Core::RenderViewRequestRegistry m_RenderViewRequestRegistry;
		Ludus::Engine::Core::SceneRegistry m_SceneRegistry;
		SystemScheduler m_Scheduler;

		RuntimeInstance(
			IHostContext& hostContext,
			RuntimeManifest RuntimeManifest,
			RuntimeEnvironment runtimeEnvironment,
			Ludus::Engine::Graphics::RenderingConfiguration2D renderingConfiguration,
			Ludus::Engine::Graphics::RenderPresentationSettings renderPresentationSettings,
			Ludus::Engine::Physics::Core::PhysicsConfiguration2D physicsConfiguration
		);

	public:
		~RuntimeInstance();

		static std::unique_ptr<RuntimeInstance> Create(
			IHostContext& hostContext,
			RuntimeManifest RuntimeManifest,
			RuntimeEnvironment runtimeEnvironment,
			Ludus::Engine::Graphics::RenderingConfiguration2D renderingConfiguration = Ludus::Engine::Graphics::RenderingConfiguration2D(),
			Ludus::Engine::Graphics::RenderPresentationSettings renderPresentationSettings = Ludus::Engine::Graphics::RenderPresentationSettings(),
			Ludus::Engine::Physics::Core::PhysicsConfiguration2D physicsConfiguration = Ludus::Engine::Physics::Core::PhysicsConfiguration2D()
		);

		void AddSystem(SystemDescriptor descriptor, std::unique_ptr<ISystem> system);
		void AddSystem(std::initializer_list<SystemDescriptor> descriptor, std::unique_ptr<ISystem> system);

		void Initialize();
		void Shutdown();

		void UpdateTransitions(const Ludus::Engine::Core::FlagSet& executionFlags);

		void Run(SystemPhase phase, Ludus::Engine::Core::FlagSet& executionFlags, float time = 0.0f);

#pragma region Getters

		Ludus::Engine::Physics::Core::PhysicsConfiguration2D& GetPhysicsConfiguration() { return m_PhysicsConfiguration; }
		const Ludus::Engine::Physics::Core::PhysicsConfiguration2D& GetPhysicsConfiguration() const { return m_PhysicsConfiguration; }

		Ludus::Engine::Graphics::RenderingConfiguration2D& GetRenderingConfiguration() { return m_RenderingConfiguration; }
		const Ludus::Engine::Graphics::RenderingConfiguration2D& GetRenderingConfiguration() const { return m_RenderingConfiguration; }

		Ludus::Engine::Graphics::RenderPresentationSettings& GetRenderPresentationSettings() { return m_RenderPresentationSettings; }
		const Ludus::Engine::Graphics::RenderPresentationSettings& GetRenderPresentationSettings() const { return m_RenderPresentationSettings; }

		Ludus::Engine::Core::RenderViewRegistry& GetRenderViewRegistry() { return m_RenderViewRegistry; }
		const Ludus::Engine::Core::RenderViewRegistry& GetRenderViewRegistry() const { return m_RenderViewRegistry; }

		Ludus::Engine::Core::RenderViewRequestRegistry& GetRenderViewRequestRegistry() { return m_RenderViewRequestRegistry; }
		const Ludus::Engine::Core::RenderViewRequestRegistry& GetRenderViewRequestRegistry() const { return m_RenderViewRequestRegistry; }

		Ludus::Engine::Core::SceneRegistry& GetSceneRegistry() { return m_SceneRegistry; }
		const Ludus::Engine::Core::SceneRegistry& GetSceneRegistry() const { return m_SceneRegistry; }

		IHostContext& GetHostContext() { return m_HostContext; }
		const IHostContext& GetHostContext() const { return m_HostContext; }

		const RuntimeEnvironment& GetRuntimeEnvironment() const { return m_RuntimeEnvironment; }
		const RuntimeManifest& GetRuntimeManifest() const { return m_RuntimeManifest; }

#pragma endregion

#pragma region Templates

		template<typename TSystem, typename... TArgs>
		void AddSystem(Ludus::Engine::Runtime::SystemDescriptor descriptor, TArgs&&... args)
		{
			auto system = std::make_unique<TSystem>(std::forward<TArgs>(args)...);
			AddSystem(descriptor, std::move(system));
		}

		template<typename TSystem, typename... TArgs>
		void AddSystem(std::initializer_list<Ludus::Engine::Runtime::SystemDescriptor> descriptors, TArgs&&... args)
		{
			auto system = std::make_unique<TSystem>(std::forward<TArgs>(args)...);
			AddSystem(descriptors, std::move(system));
		}

		template<typename TSystem, typename... TArgs>
		void AddBeginFrameSystem(TArgs&&... args)
		{
			AddSystem<TSystem>({ Ludus::Engine::Runtime::SystemPhase::BeginFrame }, std::forward<TArgs>(args)...);
		}

		template<typename TSystem, typename... TArgs>
		void AddFixedUpdateSystem(TArgs&&... args)
		{
			AddSystem<TSystem>({ Ludus::Engine::Runtime::SystemPhase::FixedUpdate }, std::forward<TArgs>(args)...);
		}

		template<typename TSystem, typename... TArgs>
		void AddRenderSystem(TArgs&&... args)
		{
			AddSystem<TSystem>({ Ludus::Engine::Runtime::SystemPhase::Render }, std::forward<TArgs>(args)...);
		}

		template<typename TSystem, typename... TArgs>
		void AddUpdateSystem(TArgs&&... args)
		{
			AddSystem<TSystem>({ Ludus::Engine::Runtime::SystemPhase::Update }, std::forward<TArgs>(args)...);
		}

		template<typename TSystem, typename... TArgs>
		void AddEndFrameSystem(TArgs&&... args)
		{
			AddSystem<TSystem>({ Ludus::Engine::Runtime::SystemPhase::EndFrame }, std::forward<TArgs>(args)...);
		}

#pragma endregion

	};
}
