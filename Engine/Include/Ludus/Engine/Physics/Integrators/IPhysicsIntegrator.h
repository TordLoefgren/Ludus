#pragma once

#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Physics/Core/PhysicsWorld2D.h>

namespace Ludus::Engine::Physics::Integrators
{
	class IPhysicsIntegrator
	{
	public:
		virtual ~IPhysicsIntegrator() = default;

		virtual void Integrate(
			Ludus::Engine::Physics::Core::PhysicsWorld2D& world,
			float fixedTime
		) = 0;
	};
}
