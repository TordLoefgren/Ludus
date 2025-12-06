#pragma once

#include <span>
#include <vector>

#include <Ludus/Engine/Physics/Broadphase/BroadphaseAABBEntry2D.h>
#include <Ludus/Engine/Physics/Broadphase/BroadphasePair2D.h>

namespace Ludus::Engine::Physics::Broadphase
{
	class IBroadphase2D
	{
	public:
		virtual ~IBroadphase2D() = default;

		virtual void ComputePairs(
			std::span<const BroadphaseAABBEntry2D> entries,
			std::vector<BroadphasePair2D>& outPairs
		) = 0;
	};
}
