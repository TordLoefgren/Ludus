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

	Ludus::Engine::Graphics::Color GetStyleColor(Color color);

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
		Text,
		TextDisabled,
		WindowBg,
		ChildBg,
		PopupBg,
		Border,
		BorderShadow,
		FrameBg,
		FrameBgHovered,
		FrameBgActive,
		TitleBg,
		TitleBgActive,
		TitleBgCollapsed,
		MenuBarBg,
		ScrollbarBg,
		ScrollbarGrab,
		ScrollbarGrabHovered,
		ScrollbarGrabActive,
		CheckMark,
		SliderGrab,
		SliderGrabActive,
		Button,
		ButtonHovered,
		ButtonActive,
		Header,
		HeaderHovered,
		HeaderActive,
		Separator,
		SeparatorHovered,
		SeparatorActive,
		ResizeGrip,
		ResizeGripHovered,
		ResizeGripActive,
		InputTextCursor,
		TabHovered,
		Tab,
		TabSelected,
		TabSelectedOverline,
		TabDimmed,
		TabDimmedSelected,
		TabDimmedSelectedOverline,
		DockingPreview,
		DockingEmptyBg,
		PlotLines,
		PlotLinesHovered,
		PlotHistogram,
		PlotHistogramHovered,
		TableHeaderBg,
		TableBorderStrong,
		TableBorderLight,
		TableRowBg,
		TableRowBgAlt,
		TextLink,
		TextSelectedBg,
		TreeLines,
		DragDropTarget,
		DragDropTargetBg,
		UnsavedMarker,
		NavCursor,
		NavWindowingHighlight,
		NavWindowingDimBg,
		ModalWindowDimBg,
		COUNT,
	};
}
