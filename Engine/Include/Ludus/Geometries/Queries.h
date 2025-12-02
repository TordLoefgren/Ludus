#pragma once

#include <Ludus/Math/AABB.h>
#include <Ludus/Math/Vector2D.h>
#include <Ludus/Physics/Broadphase/BroadphaseDebugInfo2D.h>
#include <Ludus/Physics/Narrowphase/ContactPoint2D.h>

namespace Ludus::Geometries::Queries
{
	/// <summary>
	/// Determines whether an Axis-Aligned Bounding Box (AABB) overlap happened.
	/// </summary>
	/// <param name="a">The first AABB.</param>
	/// <param name="b">The second AABB.</param>
	/// <returns>Whether the two AABB bounds are intersecting.</returns>
	inline static bool OverlapAABB(const Ludus::Math::AABB& a, const Ludus::Math::AABB& b)
	{
		const auto collisionX = std::fabs(a.Center.X - b.Center.X) <= (a.Extents.X + b.Extents.X);
		const auto collisionY = std::fabs(a.Center.Y - b.Center.Y) <= (a.Extents.Y + b.Extents.Y);

		return collisionX && collisionY;
	}

	/// <summary>
	/// Determines whether an Axis-Aligned Bounding Box (AABB) overlap happened and supplies debug information.
	/// Inspired by https://kishimotostudios.com/articles/aabb_collision/.
	/// </summary>
	/// <param name="a">The first AABB.</param>
	/// <param name="b">The second AABB.</param>
	/// <param name="info">The AABB overlap information.</param>
	/// <returns>Whether the two AABB bounds are intersecting.</returns>
	inline static bool OverlapAABB_Debug(
		const Ludus::Math::AABB& a,
		const Ludus::Math::AABB& b,
		Ludus::Physics::Broadphase::BroadphaseDebugInfo2D& info
	)
	{
		const auto maxA = a.GetMax();
		const auto minA = a.GetMin();
		const auto maxB = b.GetMax();
		const auto minB = b.GetMin();

		info.BoundsA = a;
		info.BoundsB = b;

		info.IsXAxisOverlap = std::fabs(a.Center.X - b.Center.X) <= (a.Extents.X + b.Extents.X);
		info.IsYAxisOverlap = std::fabs(a.Center.Y - b.Center.Y) <= (a.Extents.Y + b.Extents.Y);

		info.IsARightOfB = minA.X > maxB.X;
		info.IsALeftOfB = maxA.X < minB.X;
		info.IsAAboveB = minA.Y > maxB.Y;
		info.IsABelowB = maxA.Y < minB.Y;

		return info.IsXAxisOverlap && info.IsYAxisOverlap;
	}

	inline static bool IntersectionAABB_AABB(
		const Ludus::Math::AABB& a,
		const Ludus::Math::AABB& b,
		Ludus::Physics::Narrowphase::ContactPoint2D& contactPoint
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

		contactPoint = Ludus::Physics::Narrowphase::ContactPoint2D(
			Ludus::Math::Vector2D::Zero(),
			Ludus::Math::Vector2D::Zero(),
			bestAxis, penetration
		);

		return true;
	}
}
