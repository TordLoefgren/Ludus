#include "Vector2D.h"

#pragma region Constructor / Destructor

Vector2D::Vector2D()
	: X(0.0f), Y(0.0f)
{ }

Vector2D::Vector2D(float value)
	: X(value), Y(value)
{ }

Vector2D::Vector2D(float x, float y)
	: X(x), Y(y)
{ }

Vector2D::~Vector2D()
{ }

#pragma endregion

#pragma region Operator overloads (non-mutating)

Vector2D Vector2D::operator+(const Vector2D& other) const
{
	return Vector2D(X + other.X, Y + other.Y);
}

Vector2D Vector2D::operator-(const Vector2D& other) const
{
	return Vector2D(X - other.X, Y - other.Y);
}

Vector2D Vector2D::operator+(const float value) const
{
	return Vector2D(X + value, Y + value);
}

Vector2D Vector2D::operator-(const float value) const
{
	return Vector2D(X - value, Y - value);
}

Vector2D Vector2D::operator/(const float value) const
{
	if (value == 0)
	{
		throw std::runtime_error("Attempted to divide by zero.");
	}

	return Vector2D(X / value, Y / value);
}

Vector2D Vector2D::operator*(const float value) const
{
	return Vector2D(X * value, Y * value);
}

#pragma endregion

#pragma region Operator overloads (mutating)

Vector2D& Vector2D::operator+=(const Vector2D& other)
{
	X += other.X;
	Y += other.Y;

	return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& other)
{
	X -= other.X;
	Y -= other.Y;

	return *this;
}

Vector2D& Vector2D::operator+=(float value)
{
	X += value;
	Y += value;

	return *this;
}

Vector2D& Vector2D::operator-=(float value)
{
	X -= value;
	Y -= value;

	return *this;
}

Vector2D& Vector2D::operator/=(float value)
{
	if (value == 0)
	{
		throw std::runtime_error("Attempted to divide by zero.");
	}

	X /= value;
	Y /= value;

	return *this;
}

Vector2D& Vector2D::operator*=(float value)
{
	X *= value;
	Y *= value;

	return *this;
}

#pragma endregion

#pragma region Utility methods

float Vector2D::Dot(const Vector2D& v1, const Vector2D& v2)
{
	return v1.X * v2.X + v1.Y * v2.Y;
}

float Vector2D::Cross(const Vector2D& v1, const Vector2D& v2)
{
	return v1.X * v2.Y - v1.Y * v2.X;
}

float Vector2D::Magnitude(const Vector2D& v)
{
	return float(sqrt(v.X * v.X + v.Y * v.Y));
}

Vector2D Vector2D::Normalize(const Vector2D& v)
{
	auto magnitude = Magnitude(v);
	if (magnitude == 0)
	{
		return v;
	}

	return Vector2D(v.X / magnitude, v.Y / magnitude);
}

Vector2D& Vector2D::Normalize()
{
	auto magnitude = Magnitude(*this);
	if (magnitude == 0)
	{
		return *this;
	}

	X = X / magnitude;
	Y = Y / magnitude;

	return *this;
}

Vector2D Vector2D::Reflect(const Vector2D& v, const Vector2D& normal)
{
	auto normalized = Normalize(normal);
	return v - normalized * (2 * Dot(v, normalized));
}

Vector2D& Vector2D::Reflect(const Vector2D& normal)
{
	auto normalized = Normalize(normal);
	auto result = *this - normalized * (2 * Dot(*this, normalized));

	X = result.X;
	Y = result.Y;

	return *this;
}

Vector2D Vector2D::Rotate(const Vector2D& v, float angle)
{
	auto angleRadians = Numeric::DegreesToRadians(angle);
	auto x = v.X * cos(angleRadians) - v.Y * sin(angleRadians);
	auto y = v.X * sin(angleRadians) + v.Y * cos(angleRadians);

	return Vector2D((float)x, (float)y);
}

Vector2D& Vector2D::Rotate(float angle)
{
	auto angleRadians = Numeric::DegreesToRadians(angle);
	auto x = X * cos(angleRadians) - Y * sin(angleRadians);
	auto y = X * sin(angleRadians) + Y * cos(angleRadians);

	X = (float)x;
	Y = (float)y;

	return *this;
}

Vector2D Vector2D::Zero()
{
	return Vector2D();
}

#pragma endregion
