#pragma once

#include <Ludus/Math/Transform2D.h>
#include <Ludus/Physics/Core/PhysicsWorld2D.h>

namespace Ludus::Physics::Integrators
{
	class IPhysicsIntegrator
	{
	public:
		virtual ~IPhysicsIntegrator() = default;

		virtual void Integrate(
			Ludus::Physics::Core::PhysicsWorld2D& world,
			float fixedTime
		) = 0;
	};
}
