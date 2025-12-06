#pragma once

#include <Ludus/Engine/Physics/Broadphase/IBroadphase2D.h>
#include <Ludus/Engine/Physics/Broadphase/NaiveBroadphase2D.h>
#include <Ludus/Engine/Physics/Integrators/IPhysicsIntegrator.h>
#include <Ludus/Engine/Physics/Integrators/SymplecticEulerIntegrator.h>
#include <Ludus/Engine/Physics/Narrowphase/INarrowphase2D.h>
#include <Ludus/Engine/Physics/Narrowphase/NaiveNarrowphase2D.h>
#include <Ludus/Engine/Physics/Queries/IPhysicsQueryCache2D.h>
#include <Ludus/Engine/Physics/Queries/PhysicsQueryCache2D.h>
#include <Ludus/Engine/Physics/Solvers/IContactSolver2D.h>
#include <Ludus/Engine/Physics/Solvers/MTVContactSolver.h>

namespace Ludus::Engine::Physics::Core
{
	struct PhysicsContext2D
	{
		std::unique_ptr<Ludus::Engine::Physics::Broadphase::IBroadphase2D> Broadphase;
		std::unique_ptr<Ludus::Engine::Physics::Narrowphase::INarrowphase2D> Narrowphase;
		std::unique_ptr<Ludus::Engine::Physics::Queries::IPhysicsQueryCache2D> QueryCache;
		std::unique_ptr<Ludus::Engine::Physics::Solvers::IContactSolver2D> ContactSolver;
		std::unique_ptr<Ludus::Engine::Physics::Integrators::IPhysicsIntegrator> Integrator;
		int SubSteps;

		PhysicsContext2D()
			: Broadphase(std::make_unique<Ludus::Engine::Physics::Broadphase::NaiveBroadphase2D>()),
			Narrowphase(std::make_unique<Ludus::Engine::Physics::Narrowphase::NaiveNarrowphase2D>()),
			QueryCache(std::make_unique<Ludus::Engine::Physics::Queries::PhysicsQueryCache2D>()),
			ContactSolver(std::make_unique<Ludus::Engine::Physics::Solvers::MTVContactSolver>()),
			Integrator(std::make_unique<Ludus::Engine::Physics::Integrators::SymplecticEulerIntegrator>()),
			SubSteps(8)
		{ }

		PhysicsContext2D(
			std::unique_ptr<Ludus::Engine::Physics::Broadphase::IBroadphase2D> broadphase,
			std::unique_ptr<Ludus::Engine::Physics::Narrowphase::INarrowphase2D> narrowphase,
			std::unique_ptr<Ludus::Engine::Physics::Queries::IPhysicsQueryCache2D> queryCache,
			std::unique_ptr<Ludus::Engine::Physics::Solvers::IContactSolver2D> contactSolver,
			std::unique_ptr<Ludus::Engine::Physics::Integrators::IPhysicsIntegrator> integrator,
			int subSteps = 8
		) : Broadphase(std::move(broadphase)),
			Narrowphase(std::move(narrowphase)),
			QueryCache(std::move(queryCache)),
			ContactSolver(std::move(contactSolver)),
			Integrator(std::move(integrator)),
			SubSteps(subSteps)
		{ }
	};
}
