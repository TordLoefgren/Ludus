#pragma once

namespace Ludus::Engine::Physics::Broadphase
{
	struct BroadphasePair2D
	{
		std::size_t WorldIndexA;
		std::size_t WorldIndexB;
	};
}
