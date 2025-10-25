#pragma once

#include <Ludus/Core/ISystem.h>
#include <Ludus/Physics/CollisionSystem2D.h>

namespace Ludus::Physics
{
	class PhysicsSystem2D : public Ludus::Core::ISystem
	{
	private:
		CollisionSystem2D m_CollisionSystem;

	public:
		PhysicsSystem2D() = default;
		virtual ~PhysicsSystem2D() = default;

		virtual void FixedUpdate(float fixedTime) override
		{
			auto& entityComponentSystem = m_SystemContext->EntityComponentSystem;
			auto& colliderRegistry = entityComponentSystem.Colliders;
			auto& transformRegistry = entityComponentSystem.Transforms;

			m_CollisionSystem.Step(colliderRegistry, transformRegistry);

			auto& collisionInfo = m_CollisionSystem.GetCollisionInfo();
			for (auto& info : collisionInfo)
			{
				const auto ownerHandleA = info.CollisionAOwnerHandle;
				const auto ownerHandleB = info.CollisionBOwnerHandle;

				const auto& contactPoint = info.Point;

				const auto* colliderAPtr = colliderRegistry.TryGetByOwnerMutable(ownerHandleA);
				const auto* colliderBPtr = colliderRegistry.TryGetByOwnerMutable(ownerHandleB);

				if (!(colliderAPtr && colliderBPtr))
				{
					LUDUS_LOG_ERROR("[Collision Handling] Missing transform(s).");
					continue;
				}

				const auto& colliderA = *colliderAPtr;
				const auto& colliderB = *colliderBPtr;

				auto* transformAPtr = transformRegistry.TryGetByOwnerMutable(ownerHandleA);
				auto* transformBPtr = transformRegistry.TryGetByOwnerMutable(ownerHandleB);

				if (!(transformAPtr && transformBPtr))
				{
					LUDUS_LOG_ERROR("[Collision Handling] Missing colliders(s).");
					continue;
				}

				auto& transformA = *transformAPtr;
				auto& transformB = *transformBPtr;

				const auto correction = contactPoint.Normal * contactPoint.Penetration;

				m_CollisionSystem.ResolveCollision(transformAPtr, transformBPtr, colliderA.IsStatic, colliderB.IsStatic, correction);
			}
		};

		virtual void Update(float deltaTime) override
		{ };
	};
}
