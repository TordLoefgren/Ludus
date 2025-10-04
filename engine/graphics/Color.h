#pragma once

struct Color
{
	float r, g, b, a;

	constexpr Color(float r, float g, float b) : r(r), g(g), b(b), a(1.0f) { }
	constexpr Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) { }
};

namespace Colors
{
	inline constexpr Color Red { 1.0f, 0.0f, 0.0f };
	inline constexpr Color Green { 0.0f, 1.0f, 0.0f };
	inline constexpr Color Blue { 0.0f, 0.0f, 1.0f };
	inline constexpr Color Orange { 1.0f, 0.5f, 0.0f };
	inline constexpr Color Teal { 0.0f, 0.5f, 0.5f };
	inline constexpr Color Cyan { 0.0f, 1.0f, 1.0f };
	inline constexpr Color White { 1.0f, 1.0f, 1.0f };
	inline constexpr Color Black { 0.0f, 0.0f, 0.0f };
	inline constexpr Color LightGray { 0.75f, 0.75f, 0.75f };
	inline constexpr Color Gray { 0.5f, 0.5f, 0.5f };
	inline constexpr Color DarkGray { 0.25f, 0.25f, 0.25f };
}

