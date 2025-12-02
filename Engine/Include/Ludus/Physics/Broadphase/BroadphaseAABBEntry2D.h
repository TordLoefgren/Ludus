#pragma once

#include <Ludus/Math/AABB.h>

namespace Ludus::Physics::Broadphase
{
	struct BroadphaseAABBEntry2D
	{
		Ludus::Math::AABB Bounds;
		size_t WorldIndex;
	};
}
