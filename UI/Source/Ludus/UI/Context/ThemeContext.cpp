#include "pch.h"

#include <Ludus/UI/Theme/RoleColor.h>
#include <Ludus/UI/Theme/ThemeDefinition.h>
#include <Ludus/UI/Theme/ThemeId.h>
#include <Ludus/UI/Theme/Themes.h>

namespace Ludus::UI::Context::ThemeContext
{
	namespace
	{
		Ludus::UI::Theme::ThemeDefinition g_ActiveTheme = Ludus::UI::Theme::CreateLudusDarkTheme();
		std::uint64_t g_ThemeRevision = 1;
	}

	void SetActiveTheme(const Ludus::UI::Theme::ThemeDefinition& theme)
	{
		g_ActiveTheme = theme;
		++g_ThemeRevision;
	}

	void SetActiveTheme(Ludus::UI::Theme::ThemeId id)
	{
		switch (id)
		{
			case Ludus::UI::Theme::ThemeId::LudusDark:
				g_ActiveTheme = Ludus::UI::Theme::CreateLudusDarkTheme();
				break;

			case Ludus::UI::Theme::ThemeId::LudusLight:
				g_ActiveTheme = Ludus::UI::Theme::CreateLudusLightTheme();
				break;

			default:
				g_ActiveTheme = Ludus::UI::Theme::CreateLudusDarkTheme();
				break;
		}

		++g_ThemeRevision;
	}

	const Ludus::UI::Theme::ThemeDefinition& GetActiveTheme()
	{
		return g_ActiveTheme;
	}

	const Ludus::UI::Theme::ThemeId GetActiveThemeId()
	{
		return g_ActiveTheme.Id;
	}

	const Ludus::UI::Theme::ThemePalette& GetActivePalette()
	{
		return g_ActiveTheme.Palette;
	}

	const Ludus::UI::Theme::ThemeMetrics& GetActiveMetrics()
	{
		return g_ActiveTheme.Metrics;
	}

	const Ludus::Engine::Graphics::Color& GetColor(Ludus::UI::Theme::ColorRole role)
	{
		const auto& palette = g_ActiveTheme.Palette;

		using ColorRole = Ludus::UI::Theme::ColorRole;

		switch (role)
		{
			// App and shell.
			case ColorRole::AppBackground:				return palette.AppBackground;
			case ColorRole::WindowBackground:			return palette.WindowBackground;
			case ColorRole::MenuBarBackground:			return palette.MenuBarBackground;
			case ColorRole::TitleBackground:			return palette.TitleBackground;
			case ColorRole::OverlayBackground:			return palette.OverlayBackground;

				// Panels and containers.
			case ColorRole::PanelBackground:			return palette.PanelBackground;
			case ColorRole::PanelActive:				return palette.PanelActive;
			case ColorRole::SectionBackground:			return palette.SectionBackground;
			case ColorRole::TabBackground:				return palette.TabBackground;
			case ColorRole::TabActive:					return palette.TabActive;

				// Canvas and workspace.
			case ColorRole::CanvasBackground:			return palette.CanvasBackground;
			case ColorRole::CanvasSurroundBackground:	return palette.CanvasSurroundBackground;

				// Controls.
			case ColorRole::ControlBackground:			return palette.ControlBackground;
			case ColorRole::ControlHover:				return palette.ControlHover;
			case ColorRole::ControlActive:				return palette.ControlActive;

				// Rows and selection.
			case ColorRole::RowHover:					return palette.RowHover;
			case ColorRole::RowSelected:				return palette.RowSelected;
			case ColorRole::SelectionBackground:		return palette.SelectionBackground;

				// Text.
			case ColorRole::TextPrimary:				return palette.TextPrimary;
			case ColorRole::TextSecondary:				return palette.TextSecondary;
			case ColorRole::TextDisabled:				return palette.TextDisabled;

				// Borders.
			case ColorRole::BorderSubtle:				return palette.BorderSubtle;
			case ColorRole::BorderStrong:				return palette.BorderStrong;

				// Accent.
			case ColorRole::Accent:						return palette.Accent;
			case ColorRole::AccentHover:				return palette.AccentHover;
			case ColorRole::AccentActive:				return palette.AccentActive;

				// Semantic.
			case ColorRole::Success:					return palette.Success;
			case ColorRole::Warning:					return palette.Warning;
			case ColorRole::Error:						return palette.Error;

				// Axes.
			case ColorRole::AxisX:						return palette.AxisX;
			case ColorRole::AxisY:						return palette.AxisY;
			case ColorRole::AxisZ:						return palette.AxisZ;

			default:									return palette.TextPrimary;
		}
	}

	std::uint64_t GetThemeRevision()
	{
		return g_ThemeRevision;
	}
}
