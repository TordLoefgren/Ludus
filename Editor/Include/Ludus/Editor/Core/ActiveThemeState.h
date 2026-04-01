#pragma once

#include <Ludus/UI/Context/ThemeContext.h>
#include <Ludus/UI/Theme/ThemeId.h>

namespace Ludus::Editor::Core
{
	struct ActiveThemeState
	{
		Ludus::UI::Theme::ThemeId ActiveThemeId = Ludus::UI::Context::ThemeContext::GetActiveThemeId();

		bool IsActive(const Ludus::UI::Theme::ThemeId themeId) const
		{
			return ActiveThemeId == themeId;
		}
	};
}
