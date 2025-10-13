#pragma once

#include <Ludus/Math/AABB.h>
#include <Ludus/Math/Vector2D.h>
#include <Ludus/Physics/ContactPoint.h>

namespace Ludus::Physics::Narrowphase
{
	inline static bool IntersectionAABB_AABB(
		Ludus::Math::AABB& a,
		Ludus::Math::AABB& b,
		ContactPoint& contactPoint
	)
	{
		static const Ludus::Math::Vector2D faces[4] =
		{
			Ludus::Math::Vector2D(-1, 0), Ludus::Math::Vector2D(1, 0),
			Ludus::Math::Vector2D(0, -1), Ludus::Math::Vector2D(0, 1),
		};

		const auto maxA = a.GetMax();
		const auto minA = a.GetMin();

		const auto maxB = b.GetMax();
		const auto minB = b.GetMin();

		float distances[4] =
		{
			(maxB.X - minA.X),	// Distance of box B to left of A.
			(maxA.X - minB.X),	// Distance of box B to right of A.
			(maxB.Y - minA.Y),	// Distance of box B to bottom of A.
			(maxA.Y - minB.Y),	// Distance of box B to top of A.
		};
		auto penetration = FLT_MAX;
		Ludus::Math::Vector2D bestAxis;

		for (int i = 0; i < 4; i++)
		{
			if (distances[i] < penetration)
			{
				penetration = distances[i];
				bestAxis = faces[i];
			}
		}

		// Allow a small penetration threshold in order to avoid jitters. 
		const float allowance = 0.01f;
		penetration = std::max(0.f, penetration - allowance);

		contactPoint = ContactPoint(
			Ludus::Math::Vector2D::Zero(),
			Ludus::Math::Vector2D::Zero(),
			bestAxis, penetration
		);

		return true;
	}
}
