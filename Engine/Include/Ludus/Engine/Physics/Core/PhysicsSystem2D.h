#pragma once

#include <Ludus/Engine/Core/EntityComponentSystem.h>
#include <Ludus/Engine/Core/ISystem.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Physics/Broadphase/IBroadphase2D.h>
#include <Ludus/Engine/Physics/Broadphase/NaiveBroadphase2D.h>
#include <Ludus/Engine/Physics/Core/PhysicsConfiguration2D.h>
#include <Ludus/Engine/Physics/Core/PhysicsPipeline2D.h>
#include <Ludus/Engine/Physics/Core/PhysicsWorld2D.h>
#include <Ludus/Engine/Physics/Narrowphase/INarrowphase2D.h>
#include <Ludus/Engine/Physics/Narrowphase/NaiveNarrowphase2D.h>
#include <Ludus/Engine/Physics/Queries/IPhysicsQueryCache2D.h>

namespace Ludus::Engine::Physics::Core
{
	class PhysicsSystem2D final : public Ludus::Engine::Core::ISystem
	{
	private:
		PhysicsWorld2D m_PhysicsWorld;
		PhysicsPipeline2D m_PhysicsPipeline;
		int m_SubSteps;

		Ludus::Engine::Physics::Queries::IPhysicsQueryCache2D* m_Queries = nullptr;

	public:
		PhysicsSystem2D(PhysicsConfiguration2D& physicsConfiguration)
			: m_PhysicsWorld(),
			m_PhysicsPipeline(
				*physicsConfiguration.Broadphase,
				*physicsConfiguration.Narrowphase,
				*physicsConfiguration.ContactSolver,
				*physicsConfiguration.Integrator
			),
			m_Queries(physicsConfiguration.QueryCache.get()),
			m_SubSteps(physicsConfiguration.SubSteps)
		{ }

		void PullEntityComponents(Ludus::Engine::Core::EntityComponentSystem& entityComponentSystem)
		{
			m_PhysicsWorld.Clear();

			auto rigidBodies = entityComponentSystem.RigidBodies.ViewMutable();

			m_PhysicsWorld.Entities.reserve(rigidBodies.size());
			m_PhysicsWorld.Colliders.reserve(rigidBodies.size());
			m_PhysicsWorld.RigidBodies.reserve(rigidBodies.size());
			m_PhysicsWorld.Transforms.reserve(rigidBodies.size());

			for (auto& body : rigidBodies)
			{
				auto* transform = entityComponentSystem.Transforms.TryGetByOwnerMutable(body.OwnerHandle);
				if (!transform)
				{
					continue;
				}

				auto* collider = entityComponentSystem.Colliders.TryGetByOwnerMutable(body.OwnerHandle);
				if (!collider)
				{
					continue;
				}

				m_PhysicsWorld.Entities.push_back(body.OwnerHandle);
				m_PhysicsWorld.Colliders.push_back(collider);
				m_PhysicsWorld.RigidBodies.push_back(&body);
				m_PhysicsWorld.Transforms.push_back(transform);
			}
		}

		virtual void FixedUpdateImpl(float fixedTime) override
		{
			for (auto& scene : m_SystemContext->SceneRegistry.ViewMutable())
			{
				PullEntityComponents(scene.EntityComponentSystem);
				m_PhysicsPipeline.Step(m_PhysicsWorld, *m_Queries, fixedTime, m_SubSteps);
			}
		};
	};
}
