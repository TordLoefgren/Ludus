#pragma once

#include <Ludus/Engine/Graphics/Color.h>

namespace Ludus::UI::Theme
{
	struct ThemePalette
	{
		// App and shell.
		Ludus::Engine::Graphics::Color AppBackground;
		Ludus::Engine::Graphics::Color WindowBackground;
		Ludus::Engine::Graphics::Color MenuBarBackground;
		Ludus::Engine::Graphics::Color TitleBackground;
		Ludus::Engine::Graphics::Color OverlayBackground;

		// Panels and containers.
		Ludus::Engine::Graphics::Color PanelBackground;
		Ludus::Engine::Graphics::Color PanelActive;
		Ludus::Engine::Graphics::Color SectionBackground;
		Ludus::Engine::Graphics::Color TabBackground;
		Ludus::Engine::Graphics::Color TabActive;

		// Canvas and workspace.
		Ludus::Engine::Graphics::Color CanvasBackground;
		Ludus::Engine::Graphics::Color CanvasSurroundBackground;

		// Controls.
		Ludus::Engine::Graphics::Color ControlBackground;
		Ludus::Engine::Graphics::Color ControlHover;
		Ludus::Engine::Graphics::Color ControlActive;

		// Rows and selection.
		Ludus::Engine::Graphics::Color RowHover;
		Ludus::Engine::Graphics::Color RowSelected;
		Ludus::Engine::Graphics::Color SelectionBackground;

		// Text.
		Ludus::Engine::Graphics::Color TextPrimary;
		Ludus::Engine::Graphics::Color TextSecondary;
		Ludus::Engine::Graphics::Color TextDisabled;

		// Borders.
		Ludus::Engine::Graphics::Color BorderSubtle;
		Ludus::Engine::Graphics::Color BorderStrong;

		// Accent.
		Ludus::Engine::Graphics::Color Accent;
		Ludus::Engine::Graphics::Color AccentHover;
		Ludus::Engine::Graphics::Color AccentActive;

		// Semantic.
		Ludus::Engine::Graphics::Color Success;
		Ludus::Engine::Graphics::Color Warning;
		Ludus::Engine::Graphics::Color Error;

		// Axes.
		Ludus::Engine::Graphics::Color AxisX;
		Ludus::Engine::Graphics::Color AxisY;
		Ludus::Engine::Graphics::Color AxisZ;
	};
}
