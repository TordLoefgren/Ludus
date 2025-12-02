#pragma once

#include <Ludus/Math/AABB.h>

namespace Ludus::Physics::Broadphase
{
	struct BroadphaseDebugInfo2D
	{
		Ludus::Math::AABB BoundsA;
		Ludus::Math::AABB BoundsB;

		bool IsXAxisOverlap;
		bool IsYAxisOverlap;

		bool IsALeftOfB;
		bool IsARightOfB;
		bool IsABelowB;
		bool IsAAboveB;
	};
}
