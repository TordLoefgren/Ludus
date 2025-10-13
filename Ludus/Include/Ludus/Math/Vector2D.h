#pragma once

#include <sstream>
#include <stdexcept>
#include <string>

#include <Ludus/Math/Constants.h>

namespace Ludus::Math
{
	enum class Direction
	{
		Left, Right, Up, Down, None
	};

	class Vector2D
	{
	public:
		Vector2D();
		Vector2D(float value);
		Vector2D(float x, float y);
		~Vector2D();

		float X;
		float Y;

		Vector2D operator+(const Vector2D& other) const;
		Vector2D operator-(const Vector2D& other) const;

		Vector2D operator+(const float value) const;
		Vector2D operator-(const float value) const;
		Vector2D operator/(const float value) const;
		Vector2D operator*(const float value) const;

		Vector2D& operator+=(const Vector2D& other);
		Vector2D& operator-=(const Vector2D& other);

		Vector2D& operator+=(float value);
		Vector2D& operator-=(float value);
		Vector2D& operator/=(float value);
		Vector2D& operator*=(float value);

		static float Dot(const Vector2D& v1, const Vector2D& v2);
		static float Cross(const Vector2D& v1, const Vector2D& v2);
		static float Magnitude(const Vector2D& v);
		static Direction GetDirection(const Vector2D& v, float threshold = Constants::EPSILON);
		static bool IsVertical(const Vector2D& v, float threshold = Constants::EPSILON);
		static bool IsHorizontal(const Vector2D& v, float threshold = Constants::EPSILON);
		static Vector2D Normalize(const Vector2D& v);
		static Vector2D Reflect(const Vector2D& v, const Vector2D& normal);
		static Vector2D Rotate(const Vector2D& v, float angle);
		static Vector2D Zero();

		Vector2D& Normalize();
		Vector2D& Reflect(const Vector2D& normal);
		Vector2D& Rotate(float angle);

		const std::string ToString();
	};
}
