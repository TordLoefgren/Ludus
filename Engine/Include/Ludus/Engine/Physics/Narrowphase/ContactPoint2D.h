#pragma once

#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::Engine::Physics::Narrowphase
{
	struct ContactPoint2D
	{
		Ludus::Engine::Math::Vector2D LocalA;
		Ludus::Engine::Math::Vector2D LocalB;
		Ludus::Engine::Math::Vector2D Normal;
		float Penetration = 0;
	};
}
