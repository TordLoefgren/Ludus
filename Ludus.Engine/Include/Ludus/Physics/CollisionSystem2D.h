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

	class CollisionSystem2D
	{
	private:
		std::vector<CollisionInfo2D> m_Collisions;

	public:
		CollisionSystem2D() = default;
		~CollisionSystem2D() = default;

		const std::vector<CollisionInfo2D>& GetCollisionInfo(){ return m_Collisions; }
		
		void Step(const ColliderRegistry& colliderRegistry, const TransformRegistry& transformRegistry);
		void ResolveCollision(Transform2D* transformA, Transform2D* transformB, bool isStaticA, bool isStaticB, const Vector2D& correction);
	};
}
