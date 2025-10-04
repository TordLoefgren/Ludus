#pragma once

#include "ColliderSystem.h"
#include "CollisionInfo2D.h"
#include "TransformSystem.h"
#include <vector>

class Collision2DManager
{
private:
	std::vector<CollisionInfo2D> m_Collisions;

public:
	Collision2DManager() = default;
	~Collision2DManager() = default;

	const std::vector<CollisionInfo2D>& GetCollisionInfo() { return m_Collisions; }

	void Step(const ColliderSystem& colliderSystem, const TransformSystem& transformSystem);
	void ResolveCollision(Transform2D* transformA, Transform2D* transformB, bool isStaticA, bool isStaticB, const Vector2D& correction);
};
