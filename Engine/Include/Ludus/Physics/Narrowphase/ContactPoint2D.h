#pragma once

#include <Ludus/Math/Vector2D.h>

namespace Ludus::Physics::Narrowphase
{
	struct ContactPoint2D
	{
		Ludus::Math::Vector2D LocalA;
		Ludus::Math::Vector2D LocalB;
		Ludus::Math::Vector2D Normal;
		float Penetration = 0;
	};
}
