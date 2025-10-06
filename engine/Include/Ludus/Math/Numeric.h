#pragma once

#include <algorithm>

namespace Ludus::Math::Numeric
{
	inline constexpr float PI = 3.14159265358979323846f;
	inline constexpr float EPSILON = 1e-6f;

	inline float Clamp(float value, float min, float max) noexcept
	{
		return std::max(min, std::min(value, max));
	}

	inline float DegreesToRadians(float degrees) noexcept
	{
		return degrees * (PI / 180);
	}

	inline float RadiansToDegrees(float radians) noexcept
	{
		return radians * (180 / PI);
	}
}
