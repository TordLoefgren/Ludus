#pragma once

#include <Ludus/Math/AABB.h>
#include <Ludus/Physics/AABBOverlapInfo.h>

namespace Ludus::Physics::Broadphase
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
	/// Determines whether an Axis-Aligned Bounding Box (AABB) overlap happened and supplies overlap information.
	/// Inspired by https://kishimotostudios.com/articles/aabb_collision/.
	/// </summary>
	/// <param name="a">The first AABB.</param>
	/// <param name="b">The second AABB.</param>
	/// <param name="info">The AABB overlap information.</param>
	/// <returns>Whether the two AABB bounds are intersecting.</returns>
	inline static bool OverlapAABB(
		const Ludus::Math::AABB& a,
		const Ludus::Math::AABB& b,
		AABBOverlapInfo& info
	)
	{
		const auto maxA = a.GetMax();
		const auto minA = a.GetMin();

		const auto maxB = b.GetMax();
		const auto minB = b.GetMin();

		info.IsARightOfB = minA.X > maxB.X;
		info.IsALeftOfB = maxA.X < minB.X;
		info.IsAAboveB = minA.Y > maxB.Y;
		info.IsABelowB = maxA.Y < minB.Y;

		info.IsXAxisCollision = std::fabs(a.Center.X - b.Center.X) <= (a.Extents.X + b.Extents.X);
		info.IsYAxisCollision = std::fabs(a.Center.Y - b.Center.Y) <= (a.Extents.Y + b.Extents.Y);

		return info.IsXAxisCollision && info.IsYAxisCollision;
	}
}
