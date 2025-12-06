#pragma once

#include <Ludus/Engine/Math/Transform2D.h>
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
