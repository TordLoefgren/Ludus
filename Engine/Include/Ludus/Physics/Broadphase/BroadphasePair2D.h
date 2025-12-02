#pragma once

namespace Ludus::Physics::Broadphase
{
	struct BroadphasePair2D
	{
		std::size_t WorldIndexA;
		std::size_t WorldIndexB;
	};
}
