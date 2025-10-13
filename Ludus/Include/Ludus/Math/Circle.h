#pragma once

#include <sstream>
#include <string>

#include <Ludus/Math/Transform2D.h>
#include <Ludus/Math/Vector2D.h>

namespace Ludus::Math
{
	struct Circle
	{
		Vector2D Center;
		float Radius;

		static Circle FromTransform(const Transform2D& transform)
		{
			auto radius = std::min(transform.Scale.X, transform.Scale.Y) * 0.5f;
			return Circle(transform.Position, radius);
		}

		Vector2D GetDiameter() const { return Radius * 2.0f; }

		const std::string ToString()
		{
			std::ostringstream stream;
			stream << "Circle(center: (" << Center.X << ", " << Center.Y << "), radius: " << Radius << ")";

			return stream.str();
		}
	};
}
