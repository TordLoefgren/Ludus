#pragma once

#include <sstream>
#include <string>

#include <Ludus/Engine/Math/Transform2D.h>
#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::Engine::Math
{
	struct AABB
	{
		Vector2D Center;
		Vector2D Extents;

		static AABB FromTransform(const Transform2D& transform)
		{
			return AABB(transform.Position, transform.Scale / 2.0f);
		}

		Vector2D GetMin() const { return Center - Extents; }
		Vector2D GetMax() const { return Center + Extents; }
		Vector2D GetSize() const { return Extents * 2.0f; }

		const std::string ToString()
		{
			std::ostringstream stream;
			stream << "AABB(center: (" << Center.X << ", " << Center.Y << "), extents: (" << Extents.X << ", " << Extents.Y << "))";

			return stream.str();
		}
	};
}
