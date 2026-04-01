#pragma once

#include <Ludus/UI/Theme/ThemeId.h>
#include <Ludus/UI/Theme/ThemeMetrics.h>
#include <Ludus/UI/Theme/ThemePalette.h>

namespace Ludus::UI::Theme
{
	struct ThemeDefinition
	{
		ThemeId Id;
		ThemePalette Palette;
		ThemeMetrics Metrics;
	};
}
