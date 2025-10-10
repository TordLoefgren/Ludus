#pragma once

#include <cstdint>

#include <Ludus/Math/Numeric.h>
#include <Ludus/Physics/ContactPoint.h>

namespace Ludus::Physics
{
	using GameObjectHandle = uint32_t;

	struct CollisionInfo2D
	{
		GameObjectHandle CollisionAOwnerHandle;
		GameObjectHandle CollisionBOwnerHandle;
		ContactPoint Point;

		void AddContactPoint(
			const Vector2D& LocalA,
			const Vector2D& LocalB,
			const Vector2D& normal,
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
