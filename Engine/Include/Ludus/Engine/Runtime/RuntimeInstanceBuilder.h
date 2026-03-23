#pragma once

#include <functional>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include <Ludus/Engine/Graphics/RenderingConfiguration2D.h>
#include <Ludus/Engine/Graphics/RenderingOptions.h>
#include <Ludus/Engine/Graphics/RenderPresentationSettings.h>
#include <Ludus/Engine/Graphics/RenderViewConfiguration.h>
#include <Ludus/Engine/Physics/Core/PhysicsConfiguration2D.h>
#include <Ludus/Engine/Runtime/IHostContext.h>
#include <Ludus/Engine/Runtime/RuntimeEnvironment.h>
#include <Ludus/Engine/Runtime/RuntimeInstance.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>
#include <Ludus/Engine/Runtime/SystemDescriptor.h>

namespace Ludus::Engine::Runtime
{
	using RuntimeInstanceBuilderCommand = std::function<void(RuntimeInstance&)>;

	class RuntimeInstanceBuilder
	{
	private:
		Ludus::Engine::Graphics::RenderingOptions m_RenderingOptions;
		Ludus::Engine::Graphics::RenderingConfiguration2D m_RenderingConfiguration;
		Ludus::Engine::Graphics::RenderPresentationSettings m_RenderPresentationSettings;
		Ludus::Engine::Graphics::RenderViewConfiguration m_RenderViewConfiguration;
		Ludus::Engine::Physics::Core::PhysicsConfiguration2D m_PhysicsConfiguration;

		std::vector<RuntimeInstanceBuilderCommand> m_BuilderCommands;

		bool m_HasDefaultPhysics2D = false;
		bool m_HasDefaultRendering2D = false;
		bool m_HasDefaultScripting = false;

	public:
		RuntimeInstanceBuilder() = default;

		static RuntimeInstanceBuilder Create() { return RuntimeInstanceBuilder { }; }

		std::unique_ptr<Ludus::Engine::Runtime::RuntimeInstance> Build(
			IHostContext& hostContext,
			RuntimeManifest manifest,
			RuntimeEnvironment environment
		);

		RuntimeInstanceBuilder& WithRenderingConfiguration(Ludus::Engine::Graphics::RenderingConfiguration2D renderingConfiguration);
		RuntimeInstanceBuilder& WithRenderingOptions(Ludus::Engine::Graphics::RenderingOptions renderingOptions);
		RuntimeInstanceBuilder& WithRenderPresentationSettings(Ludus::Engine::Graphics::RenderPresentationSettings renderPresentationSettings);
		RuntimeInstanceBuilder& WithRenderViewConfiguration(Ludus::Engine::Graphics::RenderViewConfiguration renderViewConfiguration);
		RuntimeInstanceBuilder& WithPhysicsConfiguration(Ludus::Engine::Physics::Core::PhysicsConfiguration2D physicsConfiguration);

		RuntimeInstanceBuilder& UseDefaultPhysics2D();
		RuntimeInstanceBuilder& UseDefaultRendering2D();
		RuntimeInstanceBuilder& UseDefaultScripting();

		RuntimeInstanceBuilder& Configure(RuntimeInstanceBuilderCommand command);

#pragma region Templates

		template<typename TSystem, typename... TArgs>
		RuntimeInstanceBuilder& AddSystem(Ludus::Engine::Runtime::SystemDescriptor descriptor, TArgs&&... args)
		{
			auto capturedArgs = std::make_tuple(std::forward<TArgs>(args)...);

			m_BuilderCommands.emplace_back([descriptor, capturedArgs = std::move(capturedArgs)](Ludus::Engine::Runtime::RuntimeInstance& runtime) mutable
			{
				std::apply([&](auto&... unpackedArgs)
				{
					runtime.AddSystem<TSystem>(descriptor, unpackedArgs...);
				}, capturedArgs);
			});

			return *this;
		}

		template<typename TSystem, typename... TArgs>
		RuntimeInstanceBuilder& AddFixedUpdateSystem(TArgs&&... args)
		{
			auto capturedArgs = std::make_tuple(std::forward<TArgs>(args)...);

			m_BuilderCommands.emplace_back([capturedArgs = std::move(capturedArgs)](Ludus::Engine::Runtime::RuntimeInstance& runtime) mutable
			{
				std::apply([&](auto&... unpackedArgs)
				{
					runtime.AddFixedUpdateSystem<TSystem>(unpackedArgs...);
				}, capturedArgs);
			});

			return *this;
		}

		template<typename TSystem, typename... TArgs>
		RuntimeInstanceBuilder& AddRenderSystem(TArgs&&... args)
		{
			auto capturedArgs = std::make_tuple(std::forward<TArgs>(args)...);

			m_BuilderCommands.emplace_back([capturedArgs = std::move(capturedArgs)](Ludus::Engine::Runtime::RuntimeInstance& runtime) mutable
			{
				std::apply([&](auto&... unpackedArgs)
				{
					runtime.AddRenderSystem<TSystem>(unpackedArgs...);
				}, capturedArgs);
			});

			return *this;
		}

		template<typename TSystem, typename... TArgs>
		RuntimeInstanceBuilder& AddUpdateSystem(TArgs&&... args)
		{
			auto capturedArgs = std::make_tuple(std::forward<TArgs>(args)...);

			m_BuilderCommands.emplace_back([capturedArgs = std::move(capturedArgs)](Ludus::Engine::Runtime::RuntimeInstance& runtime) mutable
			{
				std::apply([&](auto&... unpackedArgs)
				{
					runtime.AddUpdateSystem<TSystem>(unpackedArgs...);
				}, capturedArgs);
			});

			return *this;
		}

#pragma endregion

	};
}
