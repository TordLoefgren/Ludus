#pragma once

#include <Ludus/Math/Vector2D.h>
#include <Ludus/Physics/Core/BodyType.h>
#include <Ludus/Physics/Integrators/IPhysicsIntegrator.h>

namespace Ludus::Physics::Integrators
{
	struct SymplecticEulerIntegrator final : public IPhysicsIntegrator
	{
		virtual void Integrate(
			Ludus::Physics::Core::PhysicsWorld2D& world,
			float fixedTime
		) override
		{
			for (int i = 0; i < world.Entities.size(); i++)
			{
				auto* rigidBody = world.RigidBodies[i];
				auto* transform = world.Transforms[i];

				if (rigidBody->Type == Ludus::Physics::Core::BodyType::Dynamic)
				{
					// Gravity will should be eventually be applied alongisde other forces. 
					const Ludus::Math::Vector2D gravity = { 0.0f, -10.0f };
					rigidBody->Velocity += gravity * rigidBody->GravityScale * fixedTime;
				}

				transform->Position += rigidBody->Velocity * fixedTime;
			}
		}
	};
}
