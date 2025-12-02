#pragma once

#include <Ludus/Core/ISystem.h>
#include <Ludus/Physics/Broadphase/IBroadphase2D.h>
#include <Ludus/Physics/Broadphase/NaiveBroadphase2D.h>
#include <Ludus/Physics/Core/PhysicsContext2D.h>
#include <Ludus/Physics/Core/PhysicsPipeline2D.h>
#include <Ludus/Physics/Core/PhysicsWorld2D.h>
#include <Ludus/Physics/Narrowphase/INarrowphase2D.h>
#include <Ludus/Physics/Narrowphase/NaiveNarrowphase2D.h>
#include <Ludus/Physics/Queries/IPhysicsQueryCache2D.h>

namespace Ludus::Physics::Core
{
	class PhysicsSystem2D final : public Ludus::Core::ISystem
	{
	private:
		PhysicsWorld2D m_PhysicsWorld;
		PhysicsPipeline2D m_PhysicsPipeline;
		int m_SubSteps;

		Ludus::Physics::Queries::IPhysicsQueryCache2D* m_Queries = nullptr;


	public:
		PhysicsSystem2D(PhysicsContext2D& context)
			: m_PhysicsWorld(),
			m_PhysicsPipeline(
				*context.Broadphase,
				*context.Narrowphase,
				*context.ContactSolver,
				*context.Integrator
			),
			m_Queries(context.QueryCache.get()),
			m_SubSteps(context.SubSteps)
		{ }

		void PullEntityComponents()
		{
			auto& ecs = m_SystemContext->EntityComponentSystem;

			m_PhysicsWorld.Clear();

			auto rigidBodies = ecs.RigidBodies.ViewMutable();

			m_PhysicsWorld.Entities.reserve(rigidBodies.size());
			m_PhysicsWorld.Colliders.reserve(rigidBodies.size());
			m_PhysicsWorld.RigidBodies.reserve(rigidBodies.size());
			m_PhysicsWorld.Transforms.reserve(rigidBodies.size());

			for (auto& body : rigidBodies)
			{
				auto* transform = ecs.Transforms.TryGetByOwnerMutable(body.OwnerHandle);
				if (!transform)
				{
					continue;
				}

				auto* collider = ecs.Colliders.TryGetByOwnerMutable(body.OwnerHandle);
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
			PullEntityComponents();

			m_PhysicsPipeline.Step(m_PhysicsWorld, *m_Queries, fixedTime, m_SubSteps);
		};
	};
}
