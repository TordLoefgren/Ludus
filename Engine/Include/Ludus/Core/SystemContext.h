#pragma once

#include <Ludus/Core/ResourceRegistry.h>             
#include <Ludus/Core/State.h>
#include <Ludus/Debug/Debug.h>
#include <Ludus/Engine/EntityComponentSystem.h>
#include <Ludus/Events/EventBus.h>
#include <Ludus/Physics/Core/PhysicsContext2D.h>             
#include <Ludus/Physics/Queries/IPhysicsQueryCache2D.h>       
#include <Ludus/Platform/Input.h>
#include <Ludus/Platform/Window.h>

namespace Ludus::Core
{
	struct SystemContext
	{
		Ludus::Engine::EntityComponentSystem& EntityComponentSystem;
		Ludus::Events::EventBus& EventBus;
		Ludus::Platform::Input& Input;
		Ludus::Core::ResourceRegistry& Resources;
		Ludus::Platform::Window& Window;

		Ludus::Physics::Queries::IPhysicsQueryCache2D* PhysicsQueries = nullptr;

		SystemContext(
			Ludus::Engine::EntityComponentSystem& ecs,
			Ludus::Events::EventBus& events,
			Ludus::Platform::Input& input,
			Ludus::Core::ResourceRegistry& resources,
			Ludus::Platform::Window& window,
			Ludus::Physics::Queries::IPhysicsQueryCache2D* queries = nullptr
		) : EntityComponentSystem(ecs), EventBus(events), Input(input), Window(window), Resources(resources), PhysicsQueries(queries)
		{ }

		bool HasPhysics() const
		{
			return PhysicsQueries != nullptr;
		}

		Ludus::Physics::Queries::IPhysicsQueryCache2D& GetPhysicsQueries()
		{
			LUDUS_ASSERT(PhysicsQueries, "Physics not enabled in this SystemContext");
			return *PhysicsQueries;
		}
	};
}
