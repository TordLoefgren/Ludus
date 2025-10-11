#include <iostream>

#include <Ludus/Physics/CollisionSystem2D.h>

namespace Ludus::Physics
{
	void CollisionSystem2D::Step(const ColliderRegistry& colliderRegistry, const TransformRegistry& transformRegistry)
	{
		// TODO: N^2. Should be optimized.

		m_Collisions.clear();

		auto colliders = colliderRegistry.View();

		for (size_t i = 0; i < colliders.size(); i++)
		{
			for (size_t j = i + 1; j < colliders.size(); j++)
			{
				const auto& colliderA = colliders[i];
				const auto& colliderB = colliders[j];

				// Validate static.
				if (colliderA.IsStatic && colliderB.IsStatic)
				{
					continue;
				}

				const auto transformA = transformRegistry.TryGetByOwner(colliderA.OwnerHandle);
				const auto transformB = transformRegistry.TryGetByOwner(colliderB.OwnerHandle);

				if (!transformA || !transformB)
				{
					continue;
				}

				// Validate LayerMask.
				if (!(colliderA.CollidesWith.Contains(colliderB.LayerIndex) && colliderB.CollidesWith.Contains(colliderA.LayerIndex)))
				{
					continue;
				}

				auto boundsA = colliderA.ToAABB(*transformA);
				auto boundsB = colliderB.ToAABB(*transformB);
				ContactPoint contactPoint;

				// Broad phase (AABB).
				if (boundsA.AABBIntersection(boundsB, contactPoint))
				{
					m_Collisions.emplace_back(colliderA.OwnerHandle, colliderB.OwnerHandle, contactPoint);
				}

				// TODO: Implement narrow phase.
			}
		}
	}

	void CollisionSystem2D::ResolveCollision(Transform2D* transformA, Transform2D* transformB, bool isStaticA, bool isStaticB, const Vector2D& correction)
	{
		// Resolve MTV (Minimum Translation Vector).
		if (isStaticA && !isStaticB)
		{
			// If collider A is static, push B away from A.
			transformB->Position += correction;
		}
		else if (!isStaticA && isStaticB)
		{
			// If collider B is static, push A away from B.
			transformA->Position -= correction;
		}
		else
		{
			// If both are dynamic, add half of the correction to each.
			transformA->Position -= correction * 0.5f;
			transformB->Position += correction * 0.5f;
		}
	}
}
