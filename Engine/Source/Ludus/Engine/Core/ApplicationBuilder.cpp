#include <pch.h>

#include <functional>
#include <memory>
#include <vector>

#include <Ludus/Engine/Core/ApplicationBuilder.h>
#include <Ludus/Engine/Core/ExecutionFlags.h>
#include <Ludus/Engine/Graphics/RenderViewSystem.h>

namespace Ludus::Engine::Core
{
	std::unique_ptr<Ludus::Engine::Core::Application> ApplicationBuilder::Build()
	{
		auto application = std::make_unique<Ludus::Engine::Core::Application>(
			m_ApplicationOptions,
			std::move(m_RenderingConfiguration),
			m_RenderingOptions,
			std::move(m_PhysicsConfiguration),
			m_WindowOptions
		);

		for (auto& command : m_BuilderCommands)
		{
			command(*application);
		}

		return application;
	}

	ApplicationBuilder& ApplicationBuilder::Configure(BuilderCommand command)
	{
		m_BuilderCommands.emplace_back(std::move(command));
		return *this;
	}

	ApplicationBuilder& ApplicationBuilder::WithApplicationOptions(const Ludus::Engine::Core::ApplicationOptions applicationOptions)
	{
		m_ApplicationOptions = applicationOptions;
		return *this;
	}

	ApplicationBuilder& ApplicationBuilder::WithRenderingOptions(const Ludus::Engine::Graphics::RenderingOptions renderingOptions)
	{
		m_RenderingOptions = renderingOptions;
		return *this;
	}

	ApplicationBuilder& ApplicationBuilder::WithRenderingConfiguration(Ludus::Engine::Graphics::RenderingConfiguration2D renderingConfiguration)
	{
		m_RenderingConfiguration = std::move(renderingConfiguration);
		return *this;
	}

	ApplicationBuilder& ApplicationBuilder::WithRenderViewConfiguration(Ludus::Engine::Graphics::RenderViewConfiguration renderViewConfiguration)
	{
		m_RenderViewConfiguration = renderViewConfiguration;
		return *this;
	}

	ApplicationBuilder& ApplicationBuilder::WithPhysicsConfiguration(Ludus::Engine::Physics::Core::PhysicsConfiguration2D physicsConfiguration)
	{
		m_PhysicsConfiguration = std::move(physicsConfiguration);
		return *this;
	}

	ApplicationBuilder& ApplicationBuilder::WithWindowOptions(const Ludus::Engine::Platform::WindowOptions windowOptions)
	{
		m_WindowOptions = windowOptions;
		return *this;
	}

	ApplicationBuilder& ApplicationBuilder::UseDefaultRendering2D()
	{
		if (!m_HasDefaultRendering2D)
		{
			m_BuilderCommands.emplace_back(
				[renderingOptions = m_RenderingOptions, renderViewConfiguration = m_RenderViewConfiguration](Ludus::Engine::Core::Application& application)
				{
					auto renderViewSystem = std::make_unique<Ludus::Engine::Graphics::RenderViewSystem>(renderViewConfiguration);
					auto constraints = Ludus::Engine::Core::SystemConstraints::Create()
						.RequireAnyOf(Ludus::Engine::Core::Mask(Ludus::Engine::Core::ExecutionFlags::RenderingEnabled));

					application.AddSystem({ SystemPhase::Render, SystemPhaseOrder::Before, constraints }, std::move(renderViewSystem));

					auto& renderingConfiguration = application.GetRenderingConfiguration();
					auto renderingSystem = std::make_unique<Ludus::Engine::Graphics::RenderingSystem2D>(renderingOptions, renderingConfiguration);

					application.AddSystem({ SystemPhase::Render, SystemPhaseOrder::Before }, std::move(renderingSystem));
				}
			);

			m_HasDefaultRendering2D = true;
		}
		else
		{
			LUDUS_LOG_WARN("Invalid operation: Cannot add default rendering system more than once.");
		}

		return *this;
	}

	ApplicationBuilder& ApplicationBuilder::UseDefaultPhysics2D()
	{
		if (!m_HasDefaultPhysics2D)
		{
			m_BuilderCommands.emplace_back(
				[](Ludus::Engine::Core::Application& application)
				{
					// A physics context will already have been created when this build command is invoked.
					auto& physicsConfiguration = application.GetPhysicsConfiguration();
					auto physicsSystem = std::make_unique<Ludus::Engine::Physics::Core::PhysicsSystem2D>(physicsConfiguration);
					auto constraints = Ludus::Engine::Core::SystemConstraints::Create()
						.RequireAllOf(Ludus::Engine::Core::Mask(Ludus::Engine::Core::ExecutionFlags::PhysicsEnabled))
						.RequireAnyOf(Ludus::Engine::Core::Mask(Ludus::Engine::Core::ExecutionFlags::PhysicsEnabled) | Ludus::Engine::Core::Mask(Ludus::Engine::Core::ExecutionFlags::Simulating));

					application.AddSystem({ SystemPhase::FixedUpdate, SystemPhaseOrder::Before, constraints }, std::move(physicsSystem));
				}
			);

			m_HasDefaultPhysics2D = true;
		}
		else
		{
			LUDUS_LOG_WARN("Invalid operation: Cannot add default physics system more than once.");
		}

		return *this;
	}
}
