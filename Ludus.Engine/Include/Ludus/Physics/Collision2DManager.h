#pragma once

#include <vector>

#include <Ludus/Engine/ColliderRegistry.h>
#include <Ludus/Engine/TransformRegistry.h>
#include <Ludus/Math/Vector2D.h>
#include <Ludus/Physics/CollisionInfo2D.h>

namespace Ludus::Physics
{
	using Ludus::Engine::ColliderRegistry;
	using Ludus::Engine::TransformRegistry;
	using Ludus::Math::Vector2D;

	class Collision2DManager
	{
	private:
		std::vector<CollisionInfo2D> m_Collisions;

	public:
		Collision2DManager() = default;
		~Collision2DManager() = default;

		const std::vector<CollisionInfo2D>& GetCollisionInfo() { return m_Collisions; }

		void Step(const ColliderRegistry& colliderRegistry, const TransformRegistry& transformRegistry);
		void ResolveCollision(Transform2D* transformA, Transform2D* transformB, bool isStaticA, bool isStaticB, const Vector2D& correction);
	};
}
