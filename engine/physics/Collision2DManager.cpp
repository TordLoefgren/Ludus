#include "Collision2DManager.h"
#include <iostream>

void Collision2DManager::Step(const ColliderSystem& colliderSystem, const TransformSystem& transformSystem)
{
	// TODO: N^2. Should be optimized.

	m_Collisions.clear();

	auto colliders = colliderSystem.View();

	for (size_t i = 0; i < colliders.size(); i++)
	{
		for (size_t j = i + 1; j < colliders.size(); j++)
		{
			auto& colliderA = colliders[i];
			auto& colliderB = colliders[j];

			if (colliderA.IsStatic && colliderB.IsStatic)
			{
				continue;
			}

			auto transformA = transformSystem.TryGetByOwner(colliderA.OwnerHandle);
			auto transformB = transformSystem.TryGetByOwner(colliderB.OwnerHandle);

			if (!transformA || !transformB)
			{
				continue;
			}

			// TODO: Implement check on layers and layer masks.

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
				m_Collisions.emplace_back(colliderA.Handle, colliderB.Handle, contactPoint);
			}

			// TODO: Implement narrow phase.
		}
	}
}

void Collision2DManager::ResolveCollision(Transform2D* transformA, Transform2D* transformB, bool isStaticA, bool isStaticB, const Vector2D& correction)
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
