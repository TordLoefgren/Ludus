#pragma once

#include <algorithm>

namespace Numeric
{
	inline constexpr float PI = 3.14159265358979323846f;

	inline float Clamp(float value, float min, float max)
	{
		return std::max(min, std::min(value, max));
	}

	inline float DegreesToRadians(float degrees)
	{
		return degrees * (PI / 180);
	}

	inline float RadiansToDegrees(float radians)
	{
		return radians * (180 / PI);
	}
}
