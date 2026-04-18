#pragma once

#include <stdexcept>
#include <string_view>

#include <Ludus/Engine/Core/Enums/EnumTraits.h>

namespace Ludus::UI::Theme
{
	enum class ThemeId
	{
		LudusDark,
		LudusLight
	};

	constexpr std::string_view ToString(ThemeId type)
	{
		switch (type)
		{
			case ThemeId::LudusDark:	return "LudusDark";
			case ThemeId::LudusLight:	return "LudusLight";
			default:					throw std::runtime_error("Unsupported theme id.");
		}
	}

	constexpr bool TryParse(std::string_view text, ThemeId& out)
	{
		if (text == "LudusDark")
		{
			out = ThemeId::LudusDark;
			return true;
		}

		if (text == "LudusLight")
		{
			out = ThemeId::LudusLight;
			return true;
		}

		return false;
	}
}
