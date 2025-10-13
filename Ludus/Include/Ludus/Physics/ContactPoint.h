#pragma once

#include <Ludus/Math/Vector2D.h>

namespace Ludus::Physics
{
	struct ContactPoint
	{
		Ludus::Math::Vector2D LocalA;
		Ludus::Math::Vector2D LocalB;
		Ludus::Math::Vector2D Normal;
		float Penetration;
	};
}
