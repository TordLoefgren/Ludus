#pragma once

#include <Ludus/Engine/Core/EntityComponentSystem.h>
#include <Ludus/Engine/Core/RenderViewRegistry.h>             
#include <Ludus/Engine/Core/ResourceRegistry.h>             
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Events/EventBus.h>
#include <Ludus/Engine/Graphics/RenderTarget.h>             
#include <Ludus/Engine/Physics/Queries/IPhysicsQueryCache2D.h>       
#include <Ludus/Engine/Platform/Input.h>
#include <Ludus/Engine/Platform/Window.h>

namespace Ludus::Engine::Core
{
	struct SystemContext
	{
		Ludus::Engine::Core::EntityComponentSystem& EntityComponentSystem;
		Ludus::Engine::Events::EventBus& EventBus;
		Ludus::Engine::Platform::Input& Input;
		Ludus::Engine::Core::ResourceRegistry& Resources;
		Ludus::Engine::Core::RenderViewRegistry& RenderViews;
		Ludus::Engine::Platform::Window& Window;

		std::shared_ptr<Ludus::Engine::Graphics::RenderTarget> WindowRenderTarget = nullptr;
		Ludus::Engine::Physics::Queries::IPhysicsQueryCache2D* PhysicsQueries = nullptr;

		SystemContext(
			Ludus::Engine::Core::EntityComponentSystem& ecs,
			Ludus::Engine::Events::EventBus& events,
			Ludus::Engine::Platform::Input& input,
			Ludus::Engine::Core::ResourceRegistry& resources,
			Ludus::Engine::Core::RenderViewRegistry& renderViews,
			Ludus::Engine::Platform::Window& window,
			std::shared_ptr<Ludus::Engine::Graphics::RenderTarget> windowRenderTarget = nullptr,
			Ludus::Engine::Physics::Queries::IPhysicsQueryCache2D* queries = nullptr
		) : EntityComponentSystem(ecs), EventBus(events), Input(input), Window(window), Resources(resources), RenderViews(renderViews), PhysicsQueries(queries), WindowRenderTarget(windowRenderTarget)
		{ }

		bool HasPhysics() const
		{
			return PhysicsQueries != nullptr;
		}

		Ludus::Engine::Physics::Queries::IPhysicsQueryCache2D& GetPhysicsQueries()
		{
			LUDUS_ASSERT(PhysicsQueries, "Physics not enabled in this SystemContext.");
			return *PhysicsQueries;
		}
	};
}
