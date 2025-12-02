#pragma once

namespace Ludus::Graphics
{
	struct Color
	{
		float R, G, B, A;

		constexpr Color(float r, float g, float b) : R(r), G(g), B(b), A(1.0f) { }
		constexpr Color(float r, float g, float b, float a) : R(r), G(g), B(b), A(a) { }

		constexpr Color WithAlpha(float alpha) const
		{
			return Color { R, G, B, alpha };
		}

		constexpr float* GetData()
		{
			return &R;
		};
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
		inline constexpr Color Transparent { 0.0f, 0.0f, 0.0f, 0.0f };
		inline constexpr Color Black { 0.0f, 0.0f, 0.0f };
		inline constexpr Color LightGray { 0.75f, 0.75f, 0.75f };
		inline constexpr Color Gray { 0.5f, 0.5f, 0.5f };
		inline constexpr Color DarkGray { 0.25f, 0.25f, 0.25f };
		inline constexpr Color LightBlue { 0.4f, 0.75f, 1.0f };
	}
}
