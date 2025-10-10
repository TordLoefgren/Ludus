#pragma once

#include <Ludus/Math/Vector2D.h>

namespace Ludus::Physics
{
	using Ludus::Math::Vector2D;

	struct ContactPoint
	{
		Vector2D LocalA;
		Vector2D LocalB;
		Vector2D Normal;
		float Penetration;
	};
}
