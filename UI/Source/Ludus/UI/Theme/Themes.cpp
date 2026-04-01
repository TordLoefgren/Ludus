#include "pch.h"

#include <Ludus/UI/Theme/ThemeDefinition.h>
#include <Ludus/UI/Theme/ThemeId.h>
#include <Ludus/UI/Theme/Themes.h>

namespace Ludus::UI::Theme
{
	ThemeDefinition CreateLudusDarkTheme()
	{
		ThemeDefinition theme { };

		theme.Id = ThemeId::LudusDark;

		// Palette.

		// App and shell.
		theme.Palette.AppBackground = { 0.071f, 0.078f, 0.086f, 1.000f };
		theme.Palette.WindowBackground = { 0.082f, 0.090f, 0.100f, 1.000f };
		theme.Palette.MenuBarBackground = { 0.086f, 0.095f, 0.106f, 1.000f };
		theme.Palette.TitleBackground = { 0.065f, 0.073f, 0.081f, 1.000f };
		theme.Palette.OverlayBackground = { 0.000f, 0.000f, 0.000f, 0.450f };

		// Panels and containers.
		theme.Palette.PanelBackground = { 0.094f, 0.104f, 0.115f, 1.000f };
		theme.Palette.PanelActive = { 0.135f, 0.152f, 0.170f, 1.000f };
		theme.Palette.SectionBackground = { 0.120f, 0.136f, 0.152f, 1.000f };
		theme.Palette.TabBackground = { 0.094f, 0.104f, 0.115f, 1.000f };
		theme.Palette.TabActive = { 0.135f, 0.152f, 0.170f, 1.000f };

		// Canvas and workspace.
		theme.Palette.CanvasBackground = { 0.220f, 0.220f, 0.220f, 1.000f };
		theme.Palette.CanvasSurroundBackground = { 0.055f, 0.062f, 0.070f, 1.000f };

		// Controls.
		theme.Palette.ControlBackground = { 0.122f, 0.137f, 0.153f, 1.000f };
		theme.Palette.ControlHover = { 0.160f, 0.180f, 0.200f, 1.000f };
		theme.Palette.ControlActive = { 0.176f, 0.196f, 0.220f, 1.000f };

		// Rows and selection.
		theme.Palette.RowHover = { 0.145f, 0.165f, 0.185f, 1.000f };
		theme.Palette.RowSelected = { 0.170f, 0.220f, 0.210f, 1.000f };
		theme.Palette.SelectionBackground = { 0.337f, 0.612f, 0.537f, 0.220f };

		// Text.
		theme.Palette.TextPrimary = { 0.890f, 0.910f, 0.930f, 1.000f };
		theme.Palette.TextSecondary = { 0.660f, 0.700f, 0.740f, 1.000f };
		theme.Palette.TextDisabled = { 0.430f, 0.470f, 0.520f, 1.000f };

		// Borders.
		theme.Palette.BorderSubtle = { 0.170f, 0.190f, 0.210f, 1.000f };
		theme.Palette.BorderStrong = { 0.240f, 0.265f, 0.290f, 1.000f };

		// Accent.
		theme.Palette.Accent = { 0.337f, 0.612f, 0.537f, 1.000f };
		theme.Palette.AccentHover = { 0.388f, 0.675f, 0.591f, 1.000f };
		theme.Palette.AccentActive = { 0.286f, 0.541f, 0.471f, 1.000f };

		// Semantic.
		theme.Palette.Success = { 0.420f, 0.690f, 0.500f, 1.000f };
		theme.Palette.Warning = { 0.840f, 0.660f, 0.360f, 1.000f };
		theme.Palette.Error = { 0.780f, 0.420f, 0.430f, 1.000f };

		// Axes.
		theme.Palette.AxisX = { 0.788f, 0.420f, 0.420f, 1.000f };
		theme.Palette.AxisY = { 0.470f, 0.690f, 0.530f, 1.000f };
		theme.Palette.AxisZ = { 0.450f, 0.580f, 0.820f, 1.000f };

		// Metrics.
		theme.Metrics.WindowPadding = { 8.0f, 8.0f };
		theme.Metrics.FramePadding = { 6.0f, 4.0f };
		theme.Metrics.CellPadding = { 6.0f, 4.0f };
		theme.Metrics.ItemSpacing = { 8.0f, 6.0f };
		theme.Metrics.ItemInnerSpacing = { 6.0f, 4.0f };

		theme.Metrics.WindowBorderSize = 1.0f;
		theme.Metrics.ChildBorderSize = 1.0f;
		theme.Metrics.PopupBorderSize = 1.0f;
		theme.Metrics.FrameBorderSize = 1.0f;
		theme.Metrics.TabBorderSize = 0.0f;

		theme.Metrics.WindowRounding = 4.0f;
		theme.Metrics.ChildRounding = 4.0f;
		theme.Metrics.FrameRounding = 4.0f;
		theme.Metrics.PopupRounding = 4.0f;
		theme.Metrics.ScrollbarRounding = 6.0f;
		theme.Metrics.GrabRounding = 4.0f;
		theme.Metrics.TabRounding = 4.0f;

		return theme;
	}

	ThemeDefinition CreateLudusLightTheme()
	{
		ThemeDefinition theme = CreateLudusDarkTheme();

		theme.Id = ThemeId::LudusLight;

		// App and shell.
		theme.Palette.AppBackground = { 0.880f, 0.890f, 0.905f, 1.000f };
		theme.Palette.WindowBackground = { 0.940f, 0.945f, 0.955f, 1.000f };
		theme.Palette.MenuBarBackground = { 0.915f, 0.922f, 0.932f, 1.000f };
		theme.Palette.TitleBackground = { 0.860f, 0.875f, 0.895f, 1.000f };
		theme.Palette.OverlayBackground = { 0.000f, 0.000f, 0.000f, 0.120f };

		// Panels and containers.
		theme.Palette.PanelBackground = { 0.920f, 0.928f, 0.940f, 1.000f };
		theme.Palette.PanelActive = { 0.835f, 0.850f, 0.875f, 1.000f };
		theme.Palette.SectionBackground = { 0.870f, 0.882f, 0.900f, 1.000f };
		theme.Palette.TabBackground = { 0.920f, 0.928f, 0.938f, 1.000f };
		theme.Palette.TabActive = { 0.850f, 0.865f, 0.888f, 1.000f };

		// Canvas and workspace.
		theme.Palette.CanvasBackground = { 0.300f, 0.300f, 0.300f, 1.000f };
		theme.Palette.CanvasSurroundBackground = { 0.860f, 0.870f, 0.885f, 1.000f };

		// Controls.
		theme.Palette.ControlBackground = { 0.845f, 0.860f, 0.885f, 1.000f };
		theme.Palette.ControlHover = { 0.800f, 0.820f, 0.850f, 1.000f };
		theme.Palette.ControlActive = { 0.760f, 0.785f, 0.820f, 1.000f };

		// Rows and selection.
		theme.Palette.RowHover = { 0.815f, 0.830f, 0.855f, 1.000f };
		theme.Palette.RowSelected = { 0.740f, 0.800f, 0.785f, 1.000f };
		theme.Palette.SelectionBackground = { 0.337f, 0.612f, 0.537f, 0.180f };

		// Text.
		theme.Palette.TextPrimary = { 0.120f, 0.140f, 0.165f, 1.000f };
		theme.Palette.TextSecondary = { 0.310f, 0.350f, 0.395f, 1.000f };
		theme.Palette.TextDisabled = { 0.520f, 0.560f, 0.600f, 1.000f };

		// Borders.
		theme.Palette.BorderSubtle = { 0.700f, 0.720f, 0.750f, 1.000f };
		theme.Palette.BorderStrong = { 0.640f, 0.670f, 0.710f, 1.000f };

		return theme;
	}
}
