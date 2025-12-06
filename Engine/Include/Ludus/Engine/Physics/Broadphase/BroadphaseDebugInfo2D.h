#pragma once

#include <Ludus/Engine/Math/AABB.h>

namespace Ludus::Engine::Physics::Broadphase
{
	struct BroadphaseDebugInfo2D
	{
		Ludus::Engine::Math::AABB BoundsA;
		Ludus::Engine::Math::AABB BoundsB;

		bool IsXAxisOverlap;
		bool IsYAxisOverlap;

		bool IsALeftOfB;
		bool IsARightOfB;
		bool IsABelowB;
		bool IsAAboveB;
	};
}
