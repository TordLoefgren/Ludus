#pragma once

#include <Ludus/Engine/Math/AABB.h>

namespace Ludus::Engine::Physics::Broadphase
{
	struct BroadphaseAABBEntry2D
	{
		Ludus::Engine::Math::AABB Bounds;
		size_t WorldIndex;
	};
}
