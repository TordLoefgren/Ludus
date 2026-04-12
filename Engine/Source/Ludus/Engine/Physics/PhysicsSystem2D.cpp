#include "pch.h"

#include <Ludus/Engine/Core/EntityComponentSystem.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Physics/Core/PhysicsSystem2D.h>
#include <Ludus/Engine/Physics/Queries/IPhysicsQueryCache2D.h>

namespace Ludus::Engine::Physics::Core
{
	PhysicsSystem2D::PhysicsSystem2D(
		PhysicsConfiguration2D& physicsConfiguration,
		Ludus::Engine::Core::SceneRegistry& sceneRegistry
	) :
		m_SceneRegistry(sceneRegistry),
		m_PhysicsWorld(),
		m_PhysicsPipeline(
			*physicsConfiguration.Broadphase,
			*physicsConfiguration.Narrowphase,
			*physicsConfiguration.ContactSolver,
			*physicsConfiguration.Integrator
		),
		m_SubSteps(physicsConfiguration.SubSteps),
		m_Queries(physicsConfiguration.QueryCache.get())
	{ }

	void PhysicsSystem2D::PullEntityComponents(Ludus::Engine::Core::EntityComponentSystem& entityComponentSystem)
	{
		m_PhysicsWorld.Clear();

		auto rigidBodies = entityComponentSystem.RigidBodies.ViewMutable();

		m_PhysicsWorld.Entities.reserve(rigidBodies.size());
		m_PhysicsWorld.Colliders.reserve(rigidBodies.size());
		m_PhysicsWorld.RigidBodies.reserve(rigidBodies.size());
		m_PhysicsWorld.Transforms.reserve(rigidBodies.size());

		for (auto& body : rigidBodies)
		{
			auto* transform = entityComponentSystem.Transforms.TryGetByOwnerMutable(body.OwnerId);
			if (!transform)
			{
				continue;
			}

			auto* collider = entityComponentSystem.Colliders.TryGetByOwnerMutable(body.OwnerId);
			if (!collider)
			{
				continue;
			}

			m_PhysicsWorld.Entities.push_back(body.OwnerId);
			m_PhysicsWorld.Colliders.push_back(collider);
			m_PhysicsWorld.RigidBodies.push_back(&body);
			m_PhysicsWorld.Transforms.push_back(transform);
		}
	}

	void PhysicsSystem2D::FixedUpdateImpl(float fixedTime)
	{
		m_Queries->Clear();

		for (auto& scene : m_SceneRegistry.ViewMutable())
		{
			PullEntityComponents(scene.EntityComponentSystem);
			m_PhysicsPipeline.Step(m_PhysicsWorld, *m_Queries, fixedTime, m_SubSteps);
		}
	}
}
