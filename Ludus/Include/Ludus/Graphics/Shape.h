#pragma once

#include <format>
#include <string_view>

#include <Ludus/Core/Enums.h>

namespace Ludus::Graphics
{
	enum class Shape { Rect, Circle };

	constexpr std::string_view ToString(Shape shape)
	{
		switch (shape)
		{
			case Shape::Rect:	return "Rect";
			case Shape::Circle:	return "Circle";
			default:			return "Unknown";
		}
	}
}

template <>
struct Ludus::Core::Enums::GetValues<Ludus::Graphics::Shape>
{
	static constexpr std::array<Ludus::Graphics::Shape, 2> Values {
		Ludus::Graphics::Shape::Rect,
		Ludus::Graphics::Shape::Circle
	};
};

namespace std
{
	template <>
	struct formatter<Ludus::Graphics::Shape> : formatter<string_view>
	{
		auto format(const Ludus::Graphics::Shape& shape, format_context& context) const
		{
			return formatter<string_view>::format(Ludus::Graphics::ToString(shape), context);
		}
	};
}
