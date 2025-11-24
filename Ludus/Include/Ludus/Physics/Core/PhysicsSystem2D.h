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

		Ludus::Physics::Queries::IPhysicsQueryCache2D* m_Queries = nullptr;


	public:
		PhysicsSystem2D(PhysicsContext2D& context)
			: m_PhysicsWorld(),
			m_PhysicsPipeline(
				*context.Broadphase,
				*context.Narrowphase,
				*context.ContactSolver
			),
			m_Queries(context.QueryCache.get())
		{ }

		void PullEntityComponents()
		{
			auto& ecs = m_SystemContext->EntityComponentSystem;

			m_PhysicsWorld.Clear();

			auto colliders = ecs.Colliders.ViewMutable();

			m_PhysicsWorld.Entities.reserve(colliders.size());
			m_PhysicsWorld.Colliders.reserve(colliders.size());
			m_PhysicsWorld.Transforms.reserve(colliders.size());

			for (auto& collider : colliders)
			{
				auto* transform = ecs.Transforms.TryGetByOwnerMutable(collider.OwnerHandle);
				if (!transform)
				{
					continue;
				}

				m_PhysicsWorld.Entities.push_back(collider.OwnerHandle);
				m_PhysicsWorld.Colliders.push_back(&collider);
				m_PhysicsWorld.Transforms.push_back(transform);
			}
		}

		virtual void FixedUpdateImpl(float fixedTime) override
		{
			PullEntityComponents();

			m_PhysicsPipeline.Step(m_PhysicsWorld, *m_Queries, fixedTime);
		};
	};
}
