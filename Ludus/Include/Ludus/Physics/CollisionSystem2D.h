#pragma once

#include <vector>

#include <Ludus/Engine/ColliderRegistry.h>
#include <Ludus/Engine/TransformRegistry.h>
#include <Ludus/Math/Transform2D.h>
#include <Ludus/Physics/AABBOverlapInfo.h>
#include <Ludus/Physics/Broadphase.h>
#include <Ludus/Physics/CollisionInfo2D.h>
#include <Ludus/Physics/ContactPoint.h>
#include <Ludus/Physics/Narrowphase.h>

namespace Ludus::Physics
{
	class CollisionSystem2D
	{
	private:
		std::vector<CollisionInfo2D> m_Collisions;
		std::vector<AABBOverlapInfo> m_Overlaps;

	public:
		CollisionSystem2D() = default;
		~CollisionSystem2D() = default;

		const std::vector<CollisionInfo2D>& GetCollisionInfo() { return m_Collisions; }
		const std::vector<AABBOverlapInfo>& GetOverlapInfo() { return m_Overlaps; }

		void Step(
			const Ludus::Engine::ColliderRegistry& colliderRegistry,
			const Ludus::Engine::TransformRegistry& transformRegistry,
			bool isDebug = false
		);

		void ResolveCollision(
			Ludus::Math::Transform2D* transformA,
			Ludus::Math::Transform2D* transformB,
			bool isStaticA,
			bool isStaticB,
			const Ludus::Math::Vector2D& correction
		);
	};
}
