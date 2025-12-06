#pragma once

#include <format>
#include <string_view>

#include <Ludus/Engine/Core/Enums.h>

namespace Ludus::Engine::Graphics
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
struct Ludus::Engine::Core::Enums::GetValues<Ludus::Engine::Graphics::HorizontalTextAlignment>
{
	static constexpr std::array<Ludus::Engine::Graphics::HorizontalTextAlignment, 3> Values {
		Ludus::Engine::Graphics::HorizontalTextAlignment::Left,
		Ludus::Engine::Graphics::HorizontalTextAlignment::Center,
		Ludus::Engine::Graphics::HorizontalTextAlignment::Right
	};
};

namespace std
{
	template <>
	struct formatter<Ludus::Engine::Graphics::HorizontalTextAlignment> : formatter<string_view>
	{
		auto format(const Ludus::Engine::Graphics::HorizontalTextAlignment& horizontalAlignment, format_context& context) const
		{
			return formatter<string_view>::format(Ludus::Engine::Graphics::ToString(horizontalAlignment), context);
		}
	};
}
