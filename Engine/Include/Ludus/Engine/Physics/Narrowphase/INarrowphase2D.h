#pragma once

#include <span>

#include <Ludus/Engine/Physics/Broadphase/BroadphasePair2D.h>
#include <Ludus/Engine/Physics/Core/PhysicsWorld2D.h>
#include <Ludus/Engine/Physics/Narrowphase/ContactPair2D.h>

namespace Ludus::Engine::Physics::Narrowphase
{
	class INarrowphase2D
	{
	public:
		virtual ~INarrowphase2D() = default;

		virtual void ComputeContacts(
			const Ludus::Engine::Physics::Core::PhysicsWorld2D& world,
			std::span<const Ludus::Engine::Physics::Broadphase::BroadphasePair2D> candidatePairs,
			std::vector<ContactPair2D>& outContacts
		) = 0;
	};
}
