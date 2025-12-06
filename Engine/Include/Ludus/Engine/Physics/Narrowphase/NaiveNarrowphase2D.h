#pragma once

#include <span>
#include <vector>

#include <Ludus/Engine/Geometries/Queries.h>
#include <Ludus/Engine/Physics/Core/PhysicsWorld2D.h>
#include <Ludus/Engine/Physics/Narrowphase/INarrowphase2D.h>

namespace Ludus::Engine::Physics::Narrowphase
{
	class NaiveNarrowphase2D final : public INarrowphase2D
	{
	public:
		virtual void ComputeContacts(
			const Ludus::Engine::Physics::Core::PhysicsWorld2D& world,
			std::span<const Ludus::Engine::Physics::Broadphase::BroadphasePair2D> candidatePairs,
			std::vector<ContactPair2D>& outContacts
		) override
		{
			for (const auto& pair : candidatePairs)
			{
				const auto i = pair.WorldIndexA;
				const auto j = pair.WorldIndexB;

				const auto* colliderA = world.Colliders[i];
				const auto* colliderB = world.Colliders[j];

				const auto* transformA = world.Transforms[i];
				const auto* transformB = world.Transforms[j];

				const auto canCollideAB = colliderA->CollidesWith.Contains(colliderB->LayerIndex);
				const auto canCollideBA = colliderB->CollidesWith.Contains(colliderA->LayerIndex);

				if (!canCollideAB || !canCollideBA)
				{
					continue;
				}

				const auto aabbA = colliderA->ToAABB(*transformA);
				const auto aabbB = colliderB->ToAABB(*transformB);

				ContactPoint2D contactPoint;
				if (!Ludus::Engine::Geometries::Queries::IntersectionAABB_AABB(aabbA, aabbB, contactPoint))
				{
					continue;
				}

				const auto isTrigger = colliderA->IsTrigger || colliderB->IsTrigger;

				outContacts.push_back({ colliderA->OwnerHandle, colliderB->OwnerHandle, i, j, contactPoint, isTrigger });
			}
		}
	};
}
