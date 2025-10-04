#pragma once

#include "..\graphics\Transform2D.h"
#include "..\math\Vector2D.h"
#include "CollisionInfo2D.h"

struct Bounds2D
{
	Vector2D Center;
	Vector2D Extents;

	static Bounds2D Create(const Transform2D& transform)
	{
		return Bounds2D(transform.Position, transform.Scale / 2.0f);
	}

	static Bounds2D Create(const Vector2D& position, const Vector2D& scale)
	{
		return Bounds2D(position, scale / 2.0f);
	}

	Vector2D GetMin() const { return Center - Extents; }
	Vector2D GetMax() const { return Center + Extents; }
	Vector2D GetSize() const { return Extents * 2.0f; }

	/// <summary>
	/// Determines whether an Axis-Aligned Bounding Box (AABB) intersection happened in 2D.
	/// </summary>
	/// <param name="other">The intersecting bounds.</param>
	/// <returns>Whether the two bounds are intersecting.</returns>
	bool Intersects(const Bounds2D& other) const
	{
		auto collisionX = fabs(Center.X - other.Center.X) <= (Extents.X + other.Extents.X);
		auto collisionY = fabs(Center.Y - other.Center.Y) <= (Extents.Y + other.Extents.Y);

		return collisionX && collisionY;
	}

	bool AABBTest(
		const Vector2D& posA,
		const Vector2D& posB,
		const Vector2D& halfSizeA,
		const Vector2D& halfSizeB
	)
	{
		Vector2D delta = posB - posA;
		Vector2D totalSize = halfSizeA + halfSizeB;

		if (abs(delta.X) < totalSize.X && abs(delta.Y) < totalSize.Y)
		{
			return true;
		}

		return false;
	}

	bool AABBIntersection(const Bounds2D& other, ContactPoint& contactPoint)
	{
		auto boxAPos = this->Center;
		auto boxBPos = other.Center;
		auto boxASize = this->Extents;
		auto boxBSize = other.Extents;

		auto overlap = AABBTest(boxAPos, boxBPos, boxASize, boxBSize);
		if (overlap)
		{
			static const Vector2D faces[4] =
			{
				Vector2D(-1, 0), Vector2D(1, 0),
				Vector2D(0, -1), Vector2D(0, 1),
			};

			Vector2D maxA = boxAPos + boxASize;
			Vector2D minA = boxAPos - boxASize;

			Vector2D maxB = boxBPos + boxBSize;
			Vector2D minB = boxBPos - boxBSize;

			float distances[4] =
			{
				(maxB.X - minA.X),	// Distance of box B to left of A.
				(maxA.X - minB.X),	// Distance of box B to right of A.
				(maxB.Y - minA.Y),	// Distance of box B to bottom of A.
				(maxA.Y - minB.Y),	// Distance of box B to top of A.
			};
			float penetration = FLT_MAX;
			Vector2D bestAxis;

			for (int i = 0; i < 4; i++)
			{
				if (distances[i] < penetration)
				{
					penetration = distances[i];
					bestAxis = faces[i];
				}
			}

			// Allow a small penetration threshold in order to avoid jitters. 
			const float allowance = 0.001f;
			penetration = std::max(0.f, penetration - allowance);

			contactPoint = ContactPoint(Vector2D(), Vector2D(), bestAxis, penetration);

			return true;
		}

		return false;
	}
};
