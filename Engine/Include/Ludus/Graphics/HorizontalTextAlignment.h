#pragma once

#include <format>
#include <string_view>

#include <Ludus/Core/Enums.h>

namespace Ludus::Graphics
{
	enum class HorizontalTextAlignment { Left, Center, Right };

	constexpr std::string_view ToString(HorizontalTextAlignment horizontalAlignment)
	{
		switch (horizontalAlignment)
		{
			case HorizontalTextAlignment::Left:		return "Left";
			case HorizontalTextAlignment::Center:	return "Center";
			case HorizontalTextAlignment::Right:	return "Right";
			default:								return "Unknown";
		}
	}
}

template <>
struct Ludus::Core::Enums::GetValues<Ludus::Graphics::HorizontalTextAlignment>
{
	static constexpr std::array<Ludus::Graphics::HorizontalTextAlignment, 3> Values {
		Ludus::Graphics::HorizontalTextAlignment::Left,
		Ludus::Graphics::HorizontalTextAlignment::Center,
		Ludus::Graphics::HorizontalTextAlignment::Right
	};
};

namespace std
{
	template <>
	struct formatter<Ludus::Graphics::HorizontalTextAlignment> : formatter<string_view>
	{
		auto format(const Ludus::Graphics::HorizontalTextAlignment& horizontalAlignment, format_context& context) const
		{
			return formatter<string_view>::format(Ludus::Graphics::ToString(horizontalAlignment), context);
		}
	};
}
