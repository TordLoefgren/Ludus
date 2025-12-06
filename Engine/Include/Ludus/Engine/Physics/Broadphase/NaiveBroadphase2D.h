#pragma once

#include <span>
#include <vector>

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Geometries/Queries.h>
#include <Ludus/Engine/Physics/Broadphase/BroadphaseAABBEntry2D.h>
#include <Ludus/Engine/Physics/Broadphase/BroadphasePair2D.h>
#include <Ludus/Engine/Physics/Broadphase/IBroadphase2D.h>

namespace Ludus::Engine::Physics::Broadphase
{
	class NaiveBroadphase2D final : public IBroadphase2D
	{
	public:
		virtual void ComputePairs(
			std::span<const BroadphaseAABBEntry2D> entries,
			std::vector<BroadphasePair2D>& outPairs
		) override
		{
			for (size_t i = 0; i < entries.size(); i++)
			{
				for (size_t j = i + 1; j < entries.size(); j++)
				{
					if (Ludus::Engine::Geometries::Queries::OverlapAABB(entries[i].Bounds, entries[j].Bounds))
					{
						outPairs.push_back({ entries[i].WorldIndex, entries[j].WorldIndex });
					}
				}
			}
		}
	};
}
