#pragma once

#include <Ludus/Physics/Broadphase/IBroadphase2D.h>
#include <Ludus/Physics/Broadphase/NaiveBroadphase2D.h>
#include <Ludus/Physics/Narrowphase/INarrowphase2D.h>
#include <Ludus/Physics/Narrowphase/NaiveNarrowphase2D.h>
#include <Ludus/Physics/Queries/IPhysicsQueryCache2D.h>
#include <Ludus/Physics/Queries/PhysicsQueryCache2D.h>
#include <Ludus/Physics/Solvers/IContactSolver2D.h>
#include <Ludus/Physics/Solvers/MTVContactSolver.h>

namespace Ludus::Physics::Core
{
	struct PhysicsContext2D
	{
		std::unique_ptr<Ludus::Physics::Broadphase::IBroadphase2D> Broadphase;
		std::unique_ptr<Ludus::Physics::Narrowphase::INarrowphase2D> Narrowphase;
		std::unique_ptr<Ludus::Physics::Queries::IPhysicsQueryCache2D> QueryCache;
		std::unique_ptr<Ludus::Physics::Solvers::IContactSolver2D> ContactSolver;

		PhysicsContext2D()
			: Broadphase(std::make_unique<Ludus::Physics::Broadphase::NaiveBroadphase2D>()),
			Narrowphase(std::make_unique<Ludus::Physics::Narrowphase::NaiveNarrowphase2D>()),
			QueryCache(std::make_unique<Ludus::Physics::Queries::PhysicsQueryCache2D>()),
			ContactSolver(std::make_unique<Ludus::Physics::Solvers::MTVContactSolver>())
		{ }

		PhysicsContext2D(
			std::unique_ptr<Ludus::Physics::Broadphase::IBroadphase2D> broadphase,
			std::unique_ptr<Ludus::Physics::Narrowphase::INarrowphase2D> narrowphase,
			std::unique_ptr<Ludus::Physics::Queries::IPhysicsQueryCache2D> queryCache,
			std::unique_ptr<Ludus::Physics::Solvers::IContactSolver2D> contactSolver
		) : Broadphase(std::move(broadphase)),
			Narrowphase(std::move(narrowphase)),
			QueryCache(std::move(queryCache)),
			ContactSolver(std::move(contactSolver))
		{ }
	};

}