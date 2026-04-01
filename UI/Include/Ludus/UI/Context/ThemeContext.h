#pragma once

#include <Ludus/UI/Theme/RoleColor.h>
#include <Ludus/UI/Theme/ThemeDefinition.h>
#include <Ludus/UI/Theme/ThemeId.h>
#include <Ludus/UI/Theme/ThemeMetrics.h>
#include <Ludus/UI/Theme/ThemePalette.h>

namespace Ludus::UI::Context::ThemeContext
{
	void SetActiveTheme(const Ludus::UI::Theme::ThemeDefinition& theme);
	void SetActiveTheme(Ludus::UI::Theme::ThemeId id);

	const Ludus::UI::Theme::ThemeDefinition& GetActiveTheme();
	const Ludus::UI::Theme::ThemeId GetActiveThemeId();

	const Ludus::UI::Theme::ThemePalette& GetActivePalette();
	const Ludus::UI::Theme::ThemeMetrics& GetActiveMetrics();

	const Ludus::Engine::Graphics::Color& GetColor(Ludus::UI::Theme::ColorRole role);

	std::uint64_t GetThemeRevision();

#pragma region Palette getters

	inline const Ludus::Engine::Graphics::Color& AppBackground()
	{
		return GetActivePalette().AppBackground;
	}

	inline const Ludus::Engine::Graphics::Color& WindowBackground()
	{
		return GetActivePalette().WindowBackground;
	}

	inline const Ludus::Engine::Graphics::Color& MenuBarBackground()
	{
		return GetActivePalette().MenuBarBackground;
	}

	inline const Ludus::Engine::Graphics::Color& TitleBackground()
	{
		return GetActivePalette().TitleBackground;
	}

	inline const Ludus::Engine::Graphics::Color& OverlayBackground()
	{
		return GetActivePalette().OverlayBackground;
	}

	inline const Ludus::Engine::Graphics::Color& PanelBackground()
	{
		return GetActivePalette().PanelBackground;
	}

	inline const Ludus::Engine::Graphics::Color& PanelActive()
	{
		return GetActivePalette().PanelActive;
	}

	inline const Ludus::Engine::Graphics::Color& SectionBackground()
	{
		return GetActivePalette().SectionBackground;
	}

	inline const Ludus::Engine::Graphics::Color& TabBackground()
	{
		return GetActivePalette().TabBackground;
	}

	inline const Ludus::Engine::Graphics::Color& TabActive()
	{
		return GetActivePalette().TabActive;
	}

	inline const Ludus::Engine::Graphics::Color& CanvasBackground()
	{
		return GetActivePalette().CanvasBackground;
	}

	inline const Ludus::Engine::Graphics::Color& CanvasSurroundBackground()
	{
		return GetActivePalette().CanvasSurroundBackground;
	}

	inline const Ludus::Engine::Graphics::Color& ControlBackground()
	{
		return GetActivePalette().ControlBackground;
	}

	inline const Ludus::Engine::Graphics::Color& ControlHover()
	{
		return GetActivePalette().ControlHover;
	}

	inline const Ludus::Engine::Graphics::Color& ControlActive()
	{
		return GetActivePalette().ControlActive;
	}

	inline const Ludus::Engine::Graphics::Color& RowHover()
	{
		return GetActivePalette().RowHover;
	}

	inline const Ludus::Engine::Graphics::Color& RowSelected()
	{
		return GetActivePalette().RowSelected;
	}

	inline const Ludus::Engine::Graphics::Color& SelectionBackground()
	{
		return GetActivePalette().SelectionBackground;
	}

	inline const Ludus::Engine::Graphics::Color& TextPrimary()
	{
		return GetActivePalette().TextPrimary;
	}

	inline const Ludus::Engine::Graphics::Color& TextSecondary()
	{
		return GetActivePalette().TextSecondary;
	}

	inline const Ludus::Engine::Graphics::Color& TextDisabled()
	{
		return GetActivePalette().TextDisabled;
	}

	inline const Ludus::Engine::Graphics::Color& BorderSubtle()
	{
		return GetActivePalette().BorderSubtle;
	}

	inline const Ludus::Engine::Graphics::Color& BorderStrong()
	{
		return GetActivePalette().BorderStrong;
	}

	inline const Ludus::Engine::Graphics::Color& Accent()
	{
		return GetActivePalette().Accent;
	}

	inline const Ludus::Engine::Graphics::Color& AccentHover()
	{
		return GetActivePalette().AccentHover;
	}

	inline const Ludus::Engine::Graphics::Color& AccentActive()
	{
		return GetActivePalette().AccentActive;
	}

	inline const Ludus::Engine::Graphics::Color& Success()
	{
		return GetActivePalette().Success;
	}

	inline const Ludus::Engine::Graphics::Color& Warning()
	{
		return GetActivePalette().Warning;
	}

	inline const Ludus::Engine::Graphics::Color& Error()
	{
		return GetActivePalette().Error;
	}

	inline const Ludus::Engine::Graphics::Color& AxisX()
	{
		return GetActivePalette().AxisX;
	}

	inline const Ludus::Engine::Graphics::Color& AxisY()
	{
		return GetActivePalette().AxisY;
	}

	inline const Ludus::Engine::Graphics::Color& AxisZ()
	{
		return GetActivePalette().AxisZ;
	}

#pragma endregion

}
