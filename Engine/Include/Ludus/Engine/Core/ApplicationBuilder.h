#pragma once

#include <functional>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include <Ludus/Engine/Core/Application.h>
#include <Ludus/Engine/Core/ApplicationOptions.h>
#include <Ludus/Engine/Core/SystemDescriptor.h>
#include <Ludus/Engine/Graphics/RenderingConfiguration2D.h>
#include <Ludus/Engine/Graphics/RenderingOptions.h>
#include <Ludus/Engine/Graphics/RenderPresentationSettings.h>
#include <Ludus/Engine/Graphics/RenderViewConfiguration.h>
#include <Ludus/Engine/Physics/Core/PhysicsConfiguration2D.h>
#include <Ludus/Engine/Windowing/WindowOptions.h>

namespace Ludus::Engine::Core
{
	using BuilderCommand = std::function<void(Application&)>;

	class ApplicationBuilder
	{
	private:
		Ludus::Engine::Core::ApplicationOptions m_ApplicationOptions;
		Ludus::Engine::Graphics::RenderingOptions m_RenderingOptions;
		Ludus::Engine::Graphics::RenderingConfiguration2D m_RenderingConfiguration;
		Ludus::Engine::Graphics::RenderPresentationSettings m_RenderPresentationSettings;
		Ludus::Engine::Graphics::RenderViewConfiguration m_RenderViewConfiguration;
		Ludus::Engine::Physics::Core::PhysicsConfiguration2D m_PhysicsConfiguration;
		Ludus::Engine::Windowing::WindowOptions m_WindowOptions;

		std::vector<BuilderCommand> m_BuilderCommands;

		bool m_HasDefaultPhysics2D = false;
		bool m_HasDefaultRendering2D = false;
		bool m_HasDefaultScripting = false;

	public:
		static ApplicationBuilder Create() { return ApplicationBuilder { }; }

		std::unique_ptr<Ludus::Engine::Core::Application> Build();

		ApplicationBuilder& WithApplicationOptions(Ludus::Engine::Core::ApplicationOptions applicationOptions);
		ApplicationBuilder& WithRenderingConfiguration(Ludus::Engine::Graphics::RenderingConfiguration2D renderingConfiguration);
		ApplicationBuilder& WithRenderingOptions(Ludus::Engine::Graphics::RenderingOptions renderingOptions);
		ApplicationBuilder& WithRenderPresentationSettings(Ludus::Engine::Graphics::RenderPresentationSettings renderPresentationSettings);
		ApplicationBuilder& WithRenderViewConfiguration(Ludus::Engine::Graphics::RenderViewConfiguration renderViewConfiguration);
		ApplicationBuilder& WithPhysicsConfiguration(Ludus::Engine::Physics::Core::PhysicsConfiguration2D physicsConfiguration);
		ApplicationBuilder& WithWindowOptions(Ludus::Engine::Windowing::WindowOptions windowOptions);

		ApplicationBuilder& UseDefaultPhysics2D();
		ApplicationBuilder& UseDefaultRendering2D();
		ApplicationBuilder& UseDefaultScripting();

		ApplicationBuilder& Configure(BuilderCommand command);

#pragma region Templates

		template<typename TSystem, typename... TArgs>
		ApplicationBuilder& AddSystem(Ludus::Engine::Core::SystemDescriptor descriptor, TArgs&&... args)
		{
			auto capturedArgs = std::make_tuple(std::forward<TArgs>(args)...);

			m_BuilderCommands.emplace_back([descriptor, capturedArgs = std::move(capturedArgs)](Ludus::Engine::Core::Application& application) mutable
			{
				std::apply([&](auto&... unpackedArgs)
				{
					application.AddSystem<TSystem>(descriptor, unpackedArgs...);
				}, capturedArgs);
			});

			return *this;
		}

		template<typename TSystem, typename... TArgs>
		ApplicationBuilder& AddFixedUpdateSystem(TArgs&&... args)
		{
			auto capturedArgs = std::make_tuple(std::forward<TArgs>(args)...);

			m_BuilderCommands.emplace_back([capturedArgs = std::move(capturedArgs)](Ludus::Engine::Core::Application& application) mutable
			{
				std::apply([&](auto&... unpackedArgs)
				{
					application.AddFixedUpdateSystem<TSystem>(unpackedArgs...);
				}, capturedArgs);
			});

			return *this;
		}

		template<typename TSystem, typename... TArgs>
		ApplicationBuilder& AddRenderSystem(TArgs&&... args)
		{
			auto capturedArgs = std::make_tuple(std::forward<TArgs>(args)...);

			m_BuilderCommands.emplace_back([capturedArgs = std::move(capturedArgs)](Ludus::Engine::Core::Application& application) mutable
			{
				std::apply([&](auto&... unpackedArgs)
				{
					application.AddRenderSystem<TSystem>(unpackedArgs...);
				}, capturedArgs);
			});

			return *this;
		}

		template<typename TSystem, typename... TArgs>
		ApplicationBuilder& AddUpdateSystem(TArgs&&... args)
		{
			auto capturedArgs = std::make_tuple(std::forward<TArgs>(args)...);

			m_BuilderCommands.emplace_back([capturedArgs = std::move(capturedArgs)](Ludus::Engine::Core::Application& application) mutable
			{
				std::apply([&](auto&... unpackedArgs)
				{
					application.AddUpdateSystem<TSystem>(unpackedArgs...);
				}, capturedArgs);
			});

			return *this;
		}

		template<typename T>
		ApplicationBuilder& AddResource(T&& resource)
		{
			auto capturedResource = std::forward<T>(resource);

			m_BuilderCommands.emplace_back([capturedResource = std::move(capturedResource)](Ludus::Engine::Core::Application& application) mutable
			{
				application.AddResource(capturedResource);
			});

			return *this;
		}

#pragma endregion

	};
}
