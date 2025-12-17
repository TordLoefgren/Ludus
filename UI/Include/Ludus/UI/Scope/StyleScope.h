#pragma once

#include <cstdint>
#include <initializer_list>
#include <variant>

#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::UI::Scope
{
	enum class Variable : uint32_t;

	struct StyleVar
	{
		using StyleVarData = std::variant<float, Ludus::Engine::Math::Vector2D>;

		Variable Var;
		StyleVarData Data;

		static StyleVar Float(Variable var, float value) { return { var, value }; }
		static StyleVar Vector(Variable var, Ludus::Engine::Math::Vector2D value) { return { var, value }; }
	};

	class StyleVarScope
	{
	private:
		int m_StyleCount = 0;

	public:
		explicit StyleVarScope(std::initializer_list<StyleVar> styles);

		~StyleVarScope();
	};

	enum class Variable : uint32_t
	{
		Alpha,
		DisabledAlpha,
		WindowPadding,
		WindowRounding,
		WindowBorderSize,
		WindowMinSize,
		WindowTitleAlign,
		ChildRounding,
		ChildBorderSize,
		PopupRounding,
		PopupBorderSize,
		FramePadding,
		FrameRounding,
		FrameBorderSize,
		ItemSpacing,
		ItemInnerSpacing,
		IndentSpacing,
		CellPadding,
		ScrollbarSize,
		ScrollbarRounding,
		ScrollbarPadding,
		GrabMinSize,
		GrabRounding,
		ImageBorderSize,
		TabRounding,
		TabBorderSize,
		TabMinWidthBase,
		TabMinWidthShrink,
		TabBarBorderSize,
		TabBarOverlineSize,
		TableAngledHeadersAngle,
		TableAngledHeadersTextAlign,
		TreeLinesSize,
		TreeLinesRounding,
		ButtonTextAlign,
		SelectableTextAlign,
		SeparatorTextBorderSize,
		SeparatorTextAlign,
		SeparatorTextPadding,
		DockingSeparatorSize,
		COUNT
	};

	enum class Color : uint32_t;

	struct StyleColor
	{
		Color Col;
		Ludus::Engine::Graphics::Color Value;
	};

	class StyleColorScope
	{
	private:
		int m_StyleCount = 0;

	public:
		explicit StyleColorScope(std::initializer_list<StyleColor> styles);

		~StyleColorScope();
	};

	enum class Color : uint32_t
	{
		ImGuiCol_Text,
		ImGuiCol_TextDisabled,
		ImGuiCol_WindowBg,
		ImGuiCol_ChildBg,
		ImGuiCol_PopupBg,
		ImGuiCol_Border,
		ImGuiCol_BorderShadow,
		ImGuiCol_FrameBg,
		ImGuiCol_FrameBgHovered,
		ImGuiCol_FrameBgActive,
		ImGuiCol_TitleBg,
		ImGuiCol_TitleBgActive,
		ImGuiCol_TitleBgCollapsed,
		ImGuiCol_MenuBarBg,
		ImGuiCol_ScrollbarBg,
		ImGuiCol_ScrollbarGrab,
		ImGuiCol_ScrollbarGrabHovered,
		ImGuiCol_ScrollbarGrabActive,
		ImGuiCol_CheckMark,
		ImGuiCol_SliderGrab,
		ImGuiCol_SliderGrabActive,
		ImGuiCol_Button,
		ImGuiCol_ButtonHovered,
		ImGuiCol_ButtonActive,
		ImGuiCol_Header,
		ImGuiCol_HeaderHovered,
		ImGuiCol_HeaderActive,
		ImGuiCol_Separator,
		ImGuiCol_SeparatorHovered,
		ImGuiCol_SeparatorActive,
		ImGuiCol_ResizeGrip,
		ImGuiCol_ResizeGripHovered,
		ImGuiCol_ResizeGripActive,
		ImGuiCol_InputTextCursor,
		ImGuiCol_TabHovered,
		ImGuiCol_Tab,
		ImGuiCol_TabSelected,
		ImGuiCol_TabSelectedOverline,
		ImGuiCol_TabDimmed,
		ImGuiCol_TabDimmedSelected,
		ImGuiCol_TabDimmedSelectedOverline,
		ImGuiCol_DockingPreview,
		ImGuiCol_DockingEmptyBg,
		ImGuiCol_PlotLines,
		ImGuiCol_PlotLinesHovered,
		ImGuiCol_PlotHistogram,
		ImGuiCol_PlotHistogramHovered,
		ImGuiCol_TableHeaderBg,
		ImGuiCol_TableBorderStrong,
		ImGuiCol_TableBorderLight,
		ImGuiCol_TableRowBg,
		ImGuiCol_TableRowBgAlt,
		ImGuiCol_TextLink,
		ImGuiCol_TextSelectedBg,
		ImGuiCol_TreeLines,
		ImGuiCol_DragDropTarget,
		ImGuiCol_DragDropTargetBg,
		ImGuiCol_UnsavedMarker,
		ImGuiCol_NavCursor,
		ImGuiCol_NavWindowingHighlight,
		ImGuiCol_NavWindowingDimBg,
		ImGuiCol_ModalWindowDimBg,
		ImGuiCol_COUNT,
	};
}
