#pragma once

#include <array>
#include <format>
#include <string_view>

#include <Ludus/Engine/Core/Enums.h>

namespace Ludus::Engine::Graphics
{
	enum class Shape { Circle, Quad };

	constexpr std::string_view ToString(Shape shape)
	{
		switch (shape)
		{
			case Shape::Circle:	return "Circle";
			case Shape::Quad:	return "Quad";
			default:			return "Unknown";
		}
	}

	constexpr bool TryParse(std::string_view text, Shape& out)
	{
		if (text == "Circle")
		{
			out = Shape::Circle;
			return true;
		}

		if (text == "Quad")
		{
			out = Shape::Quad;
			return true;
		}

		return false;
	}
}

template <>
struct Ludus::Engine::Core::Enums::GetValues<Ludus::Engine::Graphics::Shape>
{
	static constexpr std::array<Ludus::Engine::Graphics::Shape, 2> Values {
		Ludus::Engine::Graphics::Shape::Circle,
		Ludus::Engine::Graphics::Shape::Quad
	};
};

namespace std
{
	template <>
	struct formatter<Ludus::Engine::Graphics::Shape> : formatter<string_view>
	{
		auto format(const Ludus::Engine::Graphics::Shape& shape, format_context& context) const
		{
			return formatter<string_view>::format(Ludus::Engine::Graphics::ToString(shape), context);
		}
	};
}
