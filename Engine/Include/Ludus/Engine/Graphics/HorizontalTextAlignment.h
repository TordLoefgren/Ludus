#pragma once

#include <array>
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

	constexpr bool TryParse(std::string_view text, HorizontalTextAlignment& out)
	{
		if (text == "Left")
		{
			out = HorizontalTextAlignment::Left;
			return true;
		}

		if (text == "Center")
		{
			out = HorizontalTextAlignment::Center;
			return true;
		}

		if (text == "Right")
		{
			out = HorizontalTextAlignment::Right;
			return true;
		}

		return false;
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
