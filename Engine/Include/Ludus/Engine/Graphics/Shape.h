#pragma once

#include <format>
#include <string_view>

#include <Ludus/Engine/Core/Enums.h>

namespace Ludus::Engine::Graphics
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
struct Ludus::Engine::Core::Enums::GetValues<Ludus::Engine::Graphics::Shape>
{
	static constexpr std::array<Ludus::Engine::Graphics::Shape, 2> Values {
		Ludus::Engine::Graphics::Shape::Rect,
		Ludus::Engine::Graphics::Shape::Circle
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
