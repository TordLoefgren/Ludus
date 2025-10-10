#pragma once

#include <algorithm>
#include <tuple>

#include <Ludus/Math/Constants.h>

namespace Ludus::Math::Numeric
{
	inline float Clamp(float value, float min, float max) noexcept
	{
		return std::clamp(value, min, max);
	}

	inline float Min(float a, float b) noexcept
	{
		return std::min(a, b);
	}

	inline float Max(float a, float b) noexcept
	{
		return std::max(a, b);
	}

	inline float DegreesToRadians(float degrees) noexcept
	{
		return degrees * Constants::DEGREE_TO_RADIANS;
	}

	inline float RadiansToDegrees(float radians) noexcept
	{
		return radians * Constants::RADIANS_TO_DEGREES;
	}

	inline float RotationDegreesFromDirection(float x, float y) noexcept
	{
		return atan2(y, x) * Constants::RADIANS_TO_DEGREES;
	}

	inline std::tuple<float, float> DirectionFromRotationDegrees(float rotation) noexcept
	{
		const float r = Numeric::DegreesToRadians(rotation);
		return std::make_tuple(cos(r), sin(r));
	}
}
