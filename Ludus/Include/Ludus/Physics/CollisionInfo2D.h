#pragma once

#include <cstdint>

#include <Ludus/Math/Numeric.h>
#include <Ludus/Math/Vector2D.h>
#include <Ludus/Physics/ContactPoint.h>

namespace Ludus::Physics
{
	using EntityHandle = uint32_t;

	struct CollisionInfo2D
	{
		EntityHandle CollisionAOwnerHandle;
		EntityHandle CollisionBOwnerHandle;
		ContactPoint Point;

		void AddContactPoint(
			const Ludus::Math::Vector2D& LocalA,
			const Ludus::Math::Vector2D& LocalB,
			const Ludus::Math::Vector2D& normal,
			float point
		)
		{
			Point.LocalA = LocalA;
			Point.LocalB = LocalB;
			Point.Normal = normal;
			Point.Penetration = point;
		}
	};
}
