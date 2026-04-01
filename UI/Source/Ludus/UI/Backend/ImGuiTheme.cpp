#include "pch.h"

#include <Ludus/UI/Backend/ImGuiTheme.h>

namespace Ludus::UI::Backend
{
	namespace
	{
		ImVec4 ToImVec4(const Ludus::Engine::Graphics::Color& color)
		{
			return ImVec4(color.R, color.G, color.B, color.A);
		}

		ImVec2 ToImVec2(const Ludus::Engine::Math::Vector2D& value)
		{
			return ImVec2(value.X, value.Y);
		}

		Ludus::Engine::Graphics::Color WithAlpha(
			Ludus::Engine::Graphics::Color color,
			float alpha
		)
		{
			color.A = alpha;
			return color;
		}
	}

	void ApplyThemeToImGui(const Ludus::UI::Theme::ThemeDefinition& theme)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		const auto& palette = theme.Palette;
		const auto& metrics = theme.Metrics;

		style.WindowPadding = ToImVec2(metrics.WindowPadding);
		style.FramePadding = ToImVec2(metrics.FramePadding);
		style.CellPadding = ToImVec2(metrics.CellPadding);
		style.ItemSpacing = ToImVec2(metrics.ItemSpacing);
		style.ItemInnerSpacing = ToImVec2(metrics.ItemInnerSpacing);

		style.WindowBorderSize = metrics.WindowBorderSize;
		style.ChildBorderSize = metrics.ChildBorderSize;
		style.PopupBorderSize = metrics.PopupBorderSize;
		style.FrameBorderSize = metrics.FrameBorderSize;
		style.TabBorderSize = metrics.TabBorderSize;

		style.WindowRounding = metrics.WindowRounding;
		style.ChildRounding = metrics.ChildRounding;
		style.FrameRounding = metrics.FrameRounding;
		style.PopupRounding = metrics.PopupRounding;
		style.ScrollbarRounding = metrics.ScrollbarRounding;
		style.GrabRounding = metrics.GrabRounding;
		style.TabRounding = metrics.TabRounding;

		const ImVec4 transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		const ImVec4 dockingPreview = ToImVec4(WithAlpha(palette.Accent, 0.35f));
		const ImVec4 dragDropBackground = ToImVec4(WithAlpha(palette.Accent, 0.20f));

		colors[ImGuiCol_Text] = ToImVec4(palette.TextPrimary);
		colors[ImGuiCol_TextDisabled] = ToImVec4(palette.TextDisabled);
		colors[ImGuiCol_TextSelectedBg] = ToImVec4(palette.SelectionBackground);
		colors[ImGuiCol_TextLink] = ToImVec4(palette.AccentHover);

		colors[ImGuiCol_WindowBg] = ToImVec4(palette.WindowBackground);
		colors[ImGuiCol_ChildBg] = ToImVec4(palette.PanelBackground);
		colors[ImGuiCol_PopupBg] = ToImVec4(palette.PanelBackground);
		colors[ImGuiCol_MenuBarBg] = ToImVec4(palette.MenuBarBackground);

		colors[ImGuiCol_Border] = ToImVec4(palette.BorderSubtle);
		colors[ImGuiCol_BorderShadow] = transparent;

		colors[ImGuiCol_FrameBg] = ToImVec4(palette.ControlBackground);
		colors[ImGuiCol_FrameBgHovered] = ToImVec4(palette.ControlHover);
		colors[ImGuiCol_FrameBgActive] = ToImVec4(palette.ControlActive);

		colors[ImGuiCol_TitleBg] = ToImVec4(palette.TitleBackground);
		colors[ImGuiCol_TitleBgActive] = ToImVec4(palette.PanelActive);
		colors[ImGuiCol_TitleBgCollapsed] = ToImVec4(palette.TitleBackground);

		colors[ImGuiCol_ScrollbarBg] = ToImVec4(palette.PanelBackground);
		colors[ImGuiCol_ScrollbarGrab] = ToImVec4(palette.SectionBackground);
		colors[ImGuiCol_ScrollbarGrabHovered] = ToImVec4(palette.ControlHover);
		colors[ImGuiCol_ScrollbarGrabActive] = ToImVec4(palette.ControlActive);

		colors[ImGuiCol_CheckMark] = ToImVec4(palette.Accent);
		colors[ImGuiCol_SliderGrab] = ToImVec4(palette.Accent);
		colors[ImGuiCol_SliderGrabActive] = ToImVec4(palette.AccentHover);
		colors[ImGuiCol_InputTextCursor] = ToImVec4(palette.TextPrimary);

		colors[ImGuiCol_Button] = ToImVec4(palette.ControlBackground);
		colors[ImGuiCol_ButtonHovered] = ToImVec4(palette.ControlHover);
		colors[ImGuiCol_ButtonActive] = ToImVec4(palette.ControlActive);

		colors[ImGuiCol_Header] = ToImVec4(palette.SectionBackground);
		colors[ImGuiCol_HeaderHovered] = ToImVec4(palette.RowHover);
		colors[ImGuiCol_HeaderActive] = ToImVec4(palette.RowSelected);

		colors[ImGuiCol_Separator] = ToImVec4(palette.BorderSubtle);
		colors[ImGuiCol_SeparatorHovered] = ToImVec4(palette.AccentHover);
		colors[ImGuiCol_SeparatorActive] = ToImVec4(palette.Accent);

		colors[ImGuiCol_ResizeGrip] = ToImVec4(palette.ControlBackground);
		colors[ImGuiCol_ResizeGripHovered] = ToImVec4(palette.ControlHover);
		colors[ImGuiCol_ResizeGripActive] = ToImVec4(palette.Accent);

		colors[ImGuiCol_Tab] = ToImVec4(palette.TabBackground);
		colors[ImGuiCol_TabHovered] = ToImVec4(palette.ControlHover);
		colors[ImGuiCol_TabSelected] = ToImVec4(palette.PanelActive);
		colors[ImGuiCol_TabSelectedOverline] = ToImVec4(palette.Accent);
		colors[ImGuiCol_TabDimmed] = ToImVec4(palette.PanelBackground);
		colors[ImGuiCol_TabDimmedSelected] = ToImVec4(palette.PanelActive);
		colors[ImGuiCol_TabDimmedSelectedOverline] = ToImVec4(palette.AccentActive);

		colors[ImGuiCol_DockingPreview] = dockingPreview;
		colors[ImGuiCol_DockingEmptyBg] = ToImVec4(palette.AppBackground);

		colors[ImGuiCol_TableHeaderBg] = ToImVec4(palette.SectionBackground);
		colors[ImGuiCol_TableBorderStrong] = ToImVec4(palette.BorderStrong);
		colors[ImGuiCol_TableBorderLight] = ToImVec4(palette.BorderSubtle);
		colors[ImGuiCol_TableRowBg] = ToImVec4(palette.WindowBackground);
		colors[ImGuiCol_TableRowBgAlt] = ToImVec4(palette.PanelBackground);

		colors[ImGuiCol_PlotLines] = ToImVec4(palette.TextSecondary);
		colors[ImGuiCol_PlotLinesHovered] = ToImVec4(palette.AccentHover);
		colors[ImGuiCol_PlotHistogram] = ToImVec4(palette.TextSecondary);
		colors[ImGuiCol_PlotHistogramHovered] = ToImVec4(palette.AccentHover);

		colors[ImGuiCol_TreeLines] = ToImVec4(palette.BorderSubtle);
		colors[ImGuiCol_DragDropTarget] = ToImVec4(palette.AccentHover);
		colors[ImGuiCol_DragDropTargetBg] = dragDropBackground;
		colors[ImGuiCol_UnsavedMarker] = ToImVec4(palette.Warning);
		colors[ImGuiCol_NavCursor] = ToImVec4(palette.AccentHover);
		colors[ImGuiCol_NavWindowingHighlight] = ToImVec4(palette.Accent);
		colors[ImGuiCol_NavWindowingDimBg] = ToImVec4(palette.OverlayBackground);
		colors[ImGuiCol_ModalWindowDimBg] = ToImVec4(palette.OverlayBackground);
	}
}
