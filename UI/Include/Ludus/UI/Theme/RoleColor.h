#pragma once

namespace Ludus::UI::Theme
{
	enum class ColorRole
	{
		// App and shell.
		AppBackground,
		WindowBackground,
		MenuBarBackground,
		TitleBackground,
		OverlayBackground,

		// Panels and containers.
		PanelBackground,
		PanelActive,
		SectionBackground,
		TabBackground,
		TabActive,

		// Canvas and workspace.
		CanvasBackground,
		CanvasSurroundBackground,

		// Controls.
		ControlBackground,
		ControlHover,
		ControlActive,

		// Rows and selection.
		RowHover,
		RowSelected,
		SelectionBackground,

		// Text.
		TextPrimary,
		TextSecondary,
		TextDisabled,

		// Borders.
		BorderSubtle,
		BorderStrong,

		// Accent.
		Accent,
		AccentHover,
		AccentActive,

		// Semantic.
		Success,
		Warning,
		Error,

		// Axes.
		AxisX,
		AxisY,
		AxisZ
	};
}
