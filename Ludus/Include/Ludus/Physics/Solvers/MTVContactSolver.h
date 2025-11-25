#pragma once

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

				const auto* colliderA = world.Colliders[i];
				const auto* colliderB = world.Colliders[j];

				auto* transformA = world.Transforms[i];
				auto* transformB = world.Transforms[j];

				// Resolve MTV (Minimum Translation Vector).
				const auto correction = contact.Point.Normal * contact.Point.Penetration;

				const float weightA = Ludus::Physics::Core::GetContactWeight(colliderA->Type);
				const float weightB = Ludus::Physics::Core::GetContactWeight(colliderB->Type);

				const float weightSum = weightA + weightB;
				if (weightSum == 0.0f)
				{
					// Skip MTV.
					continue;
				}

				const float ratioA = weightA / weightSum;
				const float ratioB = weightB / weightSum;


				// Move transforms in opposite directions.
				transformA->Position -= correction * ratioA;
				transformB->Position += correction * ratioB;
			}
		}
	};
}
