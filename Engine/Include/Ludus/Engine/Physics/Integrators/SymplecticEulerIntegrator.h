#pragma once

#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/Engine/Physics/Core/BodyType.h>
#include <Ludus/Engine/Physics/Integrators/IPhysicsIntegrator.h>

namespace Ludus::Engine::Physics::Integrators
{
	struct SymplecticEulerIntegrator final : public IPhysicsIntegrator
	{
		virtual void Integrate(
			Ludus::Engine::Physics::Core::PhysicsWorld2D& world,
			float fixedTime
		) override
		{
			for (int i = 0; i < world.Entities.size(); i++)
			{
				auto* rigidBody = world.RigidBodies[i];
				auto* transform = world.Transforms[i];

				if (rigidBody->Type == Ludus::Engine::Physics::Core::BodyType::Dynamic)
				{
					// Gravity will should be eventually be applied alongisde other forces. 
					const Ludus::Engine::Math::Vector2D gravity = { 0.0f, -10.0f };
					rigidBody->Velocity += gravity * rigidBody->GravityScale * fixedTime;
				}

				transform->Position += rigidBody->Velocity * fixedTime;
			}
		}
	};
}
