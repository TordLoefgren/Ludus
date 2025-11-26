#pragma once

#include <Ludus/Math/Vector2D.h>
#include <Ludus/Physics/Solvers/IContactSolver2D.h>

namespace Ludus::Physics::Solvers
{
	class MTVContactSolver final : public IContactSolver2D
	{
		virtual void SolveContacts(
			Ludus::Physics::Core::PhysicsWorld2D& world,
			std::span<Ludus::Physics::Narrowphase::ContactPair2D> contacts
		) override
		{
			for (auto& contact : contacts)
			{
				// Triggers does not require a solver.
				if (contact.IsTriggerPair)
				{
					continue;
				}

				auto i = contact.WorldIndexA;
				auto j = contact.WorldIndexB;

				auto* bodyA = world.RigidBodies[i];
				auto* bodyB = world.RigidBodies[j];

				auto* transformA = world.Transforms[i];
				auto* transformB = world.Transforms[j];

				const float weightA = Ludus::Physics::Core::GetContactWeight(bodyA->Type);
				const float weightB = Ludus::Physics::Core::GetContactWeight(bodyB->Type);

				const float weightSum = weightA + weightB;
				if (weightSum == 0.0f)
				{
					// No movement. Skip MTV.
					continue;
				}

				const float ratioA = weightA / weightSum;
				const float ratioB = weightB / weightSum;

				// Resolve MTV (Minimum Translation Vector).
				const auto correction = contact.Point.Normal * contact.Point.Penetration;

				// Move transforms in opposite directions.
				transformA->Position -= correction * ratioA;
				transformB->Position += correction * ratioB;

				// Resolve velocities.
				const auto velocityNormalA = Ludus::Math::Vector2D::Dot(bodyA->Velocity, contact.Point.Normal);
				const auto velocityNormalB = Ludus::Math::Vector2D::Dot(bodyB->Velocity, contact.Point.Normal);

				bodyA->Velocity -= contact.Point.Normal * velocityNormalA;
				bodyB->Velocity -= contact.Point.Normal * velocityNormalB;
			}
		}
	};
}
