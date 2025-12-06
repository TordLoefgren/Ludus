#pragma once

#include <span>

#include <Ludus/Engine/Physics/Core/PhysicsWorld2D.h>
#include <Ludus/Engine/Physics/Narrowphase/ContactPair2D.h>

namespace Ludus::Engine::Physics::Solvers
{
	class IContactSolver2D
	{
	public:
		virtual ~IContactSolver2D() = default;

		virtual void SolveContacts(
			Ludus::Engine::Physics::Core::PhysicsWorld2D& world,
			std::span<Ludus::Engine::Physics::Narrowphase::ContactPair2D> contacts
		) = 0;
	};
}
