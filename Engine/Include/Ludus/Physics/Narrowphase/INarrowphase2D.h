#pragma once

#include <span>

#include <Ludus/Physics/Broadphase/BroadphasePair2D.h>
#include <Ludus/Physics/Core/PhysicsWorld2D.h>
#include <Ludus/Physics/Narrowphase/ContactPair2D.h>

namespace Ludus::Physics::Narrowphase
{
	class INarrowphase2D
	{
	public:
		virtual ~INarrowphase2D() = default;

		virtual void ComputeContacts(
			const Ludus::Physics::Core::PhysicsWorld2D& world,
			std::span<const Ludus::Physics::Broadphase::BroadphasePair2D> candidatePairs,
			std::vector<ContactPair2D>& outContacts
		) = 0;
	};
}
