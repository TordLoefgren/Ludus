#pragma once

#include <vector>

#include <Ludus/Engine/ComponentRegistry.h>
#include <Ludus/Math/AABB.h>
#include <Ludus/Math/Transform2D.h>
#include <Ludus/Physics/Broadphase/BroadphaseAABBEntry2D.h>
#include <Ludus/Physics/Broadphase/BroadphasePair2D.h>
#include <Ludus/Physics/Broadphase/IBroadphase2D.h>
#include <Ludus/Physics/Core/Collider2D.h>
#include <Ludus/Physics/Core/PhysicsWorld2D.h>
#include <Ludus/Physics/Integrators/IPhysicsIntegrator.h>
#include <Ludus/Physics/Narrowphase/ContactPair2D.h>
#include <Ludus/Physics/Narrowphase/INarrowphase2D.h>
#include <Ludus/Physics/Queries/IPhysicsQueryCache2D.h>
#include <Ludus/Physics/Solvers/IContactSolver2D.h>

namespace Ludus::Physics::Core
{
	struct PhysicsPipeline2D
	{
	private:
		Ludus::Physics::Broadphase::IBroadphase2D& m_Broadphase;
		Ludus::Physics::Narrowphase::INarrowphase2D& m_Narrowphase;
		Ludus::Physics::Solvers::IContactSolver2D& m_ContactSolver;
		Ludus::Physics::Integrators::IPhysicsIntegrator& m_Integrator;

		std::vector<Ludus::Physics::Broadphase::BroadphaseAABBEntry2D> m_AABBEntries;
		std::vector<Ludus::Physics::Broadphase::BroadphasePair2D> m_BroadphasePairs;
		std::vector<Ludus::Physics::Narrowphase::ContactPair2D> m_ContactPairs;

		void GetAABBs(
			const Ludus::Physics::Core::PhysicsWorld2D& world,
			std::vector<Ludus::Physics::Broadphase::BroadphaseAABBEntry2D>& outAABBEntries
		)
		{
			outAABBEntries.clear();
			outAABBEntries.reserve(world.Size());

			for (size_t i = 0; i < world.Size(); ++i)
			{
				auto* collider = world.Colliders[i];
				auto* transform = world.Transforms[i];

				const auto aabb = collider->ToAABB(*transform);
				outAABBEntries.push_back({ aabb, i });
			}
		}

	public:
		PhysicsPipeline2D(
			Ludus::Physics::Broadphase::IBroadphase2D& broadphase,
			Ludus::Physics::Narrowphase::INarrowphase2D& narrowphase,
			Ludus::Physics::Solvers::IContactSolver2D& contactSolver,
			Ludus::Physics::Integrators::IPhysicsIntegrator& integrator
		) : m_Broadphase(broadphase),
			m_Narrowphase(narrowphase),
			m_ContactSolver(contactSolver),
			m_Integrator(integrator)
		{ }

		~PhysicsPipeline2D() = default;

		void Step(
			Ludus::Physics::Core::PhysicsWorld2D& world,
			Ludus::Physics::Queries::IPhysicsQueryCache2D& queryCache,
			float fixedTime,
			int subSteps
		)
		{
			queryCache.Clear();

			auto fixedTimeDelta = fixedTime / subSteps;
			for (int i = 0; i < subSteps; i++)
			{
				SubStep(world, queryCache, fixedTimeDelta);
			}
		}

		void SubStep(
			Ludus::Physics::Core::PhysicsWorld2D& world,
			Ludus::Physics::Queries::IPhysicsQueryCache2D& queryCache,
			float fixedTime
		)
		{
			// Compute AABBs.
			m_AABBEntries.clear();
			GetAABBs(world, m_AABBEntries);

			// Compute Broadphase (AABBs -> Candidate pairs).
			m_BroadphasePairs.clear();
			m_Broadphase.ComputePairs(m_AABBEntries, m_BroadphasePairs);

			// Compute Narrowphase (Candidate pairs -> Contact pairs).
			m_ContactPairs.clear();
			m_Narrowphase.ComputeContacts(world, m_BroadphasePairs, m_ContactPairs);

			// Solve constraints.
			m_ContactSolver.SolveContacts(world, m_ContactPairs);

			// Populate query cache with accumulated contacts.
			queryCache.UpdateFromContacts(m_ContactPairs);

			// Integrate velocities on rigid bodies and apply to transforms.
			m_Integrator.Integrate(world, fixedTime);
		}
	};
}
