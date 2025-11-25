#pragma once

#include <span>

#include <Ludus/Physics/Core/PhysicsWorld2D.h>
#include <Ludus/Physics/Narrowphase/ContactPair2D.h>

namespace Ludus::Physics::Solvers
{
	class IContactSolver2D
	{
	public:
		virtual ~IContactSolver2D() = default;

		virtual void SolveContacts(
			Ludus::Physics::Core::PhysicsWorld2D& world,
			std::span<Ludus::Physics::Narrowphase::ContactPair2D> contacts
		) = 0;
	};
}
