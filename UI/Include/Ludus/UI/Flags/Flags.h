#pragma once

#include <cstdint>

namespace Ludus::UI::Flags
{
	enum class ColorEdit : uint32_t
	{
		None = 0,
		NoAlpha = 1 << 1,
		NoPicker = 1 << 2,
		NoOptions = 1 << 3,
		NoSmallPreview = 1 << 4,
		NoInputs = 1 << 5,
		NoTooltip = 1 << 6,
		NoLabel = 1 << 7,
		NoSidePreview = 1 << 8,
		NoDragDrop = 1 << 9,
		NoBorder = 1 << 10,

		AlphaOpaque = 1 << 11,
		AlphaNoBg = 1 << 12,
		AlphaPreviewHalf = 1 << 13,

		AlphaBar = 1 << 16,
		HDR = 1 << 19,
		DisplayRGB = 1 << 20,
		DisplayHSV = 1 << 21,
		DisplayHex = 1 << 22,
		Uint8 = 1 << 23,
		Float = 1 << 24,
		PickerHueBar = 1 << 25,
		PickerHueWheel = 1 << 26,
		InputRGB = 1 << 27,
		InputHSV = 1 << 28,

		DefaultOptions = Uint8 | DisplayRGB | InputRGB | PickerHueBar,
	};

	enum class Combo : uint32_t
	{
		None = 0,
		PopupAlignLeft = 1 << 0,
		HeightSmall = 1 << 1,
		HeightRegular = 1 << 2,
		HeightLarge = 1 << 3,
		HeightLargest = 1 << 4,
		NoArrowButton = 1 << 5,
		NoPreview = 1 << 6,
		WidthFitPreview = 1 << 7,
		HeightMask_ = HeightSmall | HeightRegular | HeightLarge | HeightLargest,
	};

	enum class DockNode : uint32_t
	{
		None = 0,
		KeepAliveOnly = 1 << 0,
		NoCentralNode = 1 << 1,
		NoDockingOverCentralNode = 1 << 2,
		PassthruCentralNode = 1 << 3,
		NoDockingSplit = 1 << 4,
		NoResize = 1 << 5,
		AutoHideTabBar = 1 << 6,
		NoUndocking = 1 << 7,
	};

	enum class DockNodeInternal : uint32_t
	{
		NoWindowMenuButton = 1 << 14
	};

	enum class Hovered : uint32_t
	{
		None = 0,

		ChildWindows = 1u << 0,
		RootWindow = 1u << 1,
		AnyWindow = 1u << 2,

		NoPopupHierarchy = 1u << 3,
		DockHierarchy = 1u << 4,

		AllowWhenBlockedByPopup = 1u << 5,
		AllowWhenBlockedByModal = 1u << 6,
		AllowWhenBlockedByActiveItem = 1u << 7,

		AllowWhenOverlappedByItem = 1u << 8,
		AllowWhenOverlappedByWindow = 1u << 9,

		AllowWhenDisabled = 1u << 10,
		NoNavOverride = 1u << 11,

		AllowWhenOverlapped = AllowWhenOverlappedByItem | AllowWhenOverlappedByWindow,
		RectOnly = AllowWhenBlockedByPopup | AllowWhenBlockedByActiveItem | AllowWhenOverlapped,
		RootAndChildWindows = RootWindow | ChildWindows,

		ForTooltip = 1u << 12,
		Stationary = 1u << 13,

		DelayNone = 1u << 14,
		DelayShort = 1u << 15,
		DelayNormal = 1u << 16,
		NoSharedDelay = 1u << 17,
	};

	enum class InputText : uint32_t
	{
		None = 0,
		CharsDecimal = 1 << 0,
		CharsHexadecimal = 1 << 1,
		CharsScientific = 1 << 2,
		CharsUppercase = 1 << 3,
		CharsNoBlank = 1 << 4,

		AllowTabInput = 1 << 5,
		EnterReturnsTrue = 1 << 6,
		EscapeClearsAll = 1 << 7,
		CtrlEnterForNewLine = 1 << 8,

		ReadOnly = 1 << 9,
		Password = 1 << 10,
		AlwaysOverwrite = 1 << 11,
		AutoSelectAll = 1 << 12,
		ParseEmptyRefVal = 1 << 13,
		DisplayEmptyRefVal = 1 << 14,
		NoHorizontalScroll = 1 << 15,
		NoUndoRedo = 1 << 16,

		ElideLeft = 1 << 17,

		CallbackCompletion = 1 << 18,
		CallbackHistory = 1 << 19,
		CallbackAlways = 1 << 20,
		CallbackCharFilter = 1 << 21,
		CallbackResize = 1 << 22,
		CallbackEdit = 1 << 23,

		WordWrap = 1 << 24,
	};

	enum class Popup : uint32_t
	{
		None = 0,

		MouseButtonLeft = 0,			// Mouse button index.
		MouseButtonRight = 1,			// Mouse button index.
		MouseButtonMiddle = 2,			// Mouse button index.

		MouseButtonMask = 0x1Fu,
		MouseButtonDefault = 1u,

		NoReopen = 1u << 5,
		NoReopenAlwaysNavInit = 1u << 6,
		NoOpenOverExistingPopup = 1u << 7,
		NoOpenOverItems = 1u << 8,

		AnyPopupId = 1u << 10,
		AnyPopupLevel = 1u << 11,
		AnyPopup = AnyPopupId | AnyPopupLevel,
	};

	enum class TabBar : uint32_t
	{
		None = 0,
		Reorderable = 1 << 0,
		AutoSelectNewTabs = 1 << 1,
		TabListPopupButton = 1 << 2,
		NoCloseWithMiddleMouseButton = 1 << 3,
		NoTabListScrollingButtons = 1 << 4,
		NoTooltip = 1 << 5,
		DrawSelectedOverline = 1 << 6,
		FittingPolicyMixed = 1 << 7,
		FittingPolicyShrink = 1 << 8,
		FittingPolicyScroll = 1 << 9,
		FittingPolicyMask_ = FittingPolicyMixed | FittingPolicyShrink | FittingPolicyScroll,
		FittingPolicyDefault_ = FittingPolicyMixed,
	};

	enum class TabItem : uint32_t
	{
		None = 0,
		UnsavedDocument = 1 << 0,
		SetSelected = 1 << 1,
		NoCloseWithMiddleMouseButton = 1 << 2,
		NoPushId = 1 << 3,
		NoTooltip = 1 << 4,
		NoReorder = 1 << 5,
		Leading = 1 << 6,
		Trailing = 1 << 7,
		NoAssumedClosure = 1 << 8,
	};

	enum class Table : uint32_t
	{
		None = 0,

		Resizable = 1u << 0,
		Reorderable = 1u << 1,
		Hideable = 1u << 2,
		Sortable = 1u << 3,

		NoSavedSettings = 1u << 4,
		ContextMenuInBody = 1u << 5,
		RowBg = 1u << 6,

		BordersInnerH = 1u << 7,
		BordersOuterH = 1u << 8,
		BordersInnerV = 1u << 9,
		BordersOuterV = 1u << 10,

		BordersH = BordersInnerH | BordersOuterH,
		BordersV = BordersInnerV | BordersOuterV,
		BordersInner = BordersInnerV | BordersInnerH,
		BordersOuter = BordersOuterV | BordersOuterH,
		Borders = BordersInner | BordersOuter,

		NoBordersInBody = 1u << 11,
		NoBordersInBodyUntilResize = 1u << 12,

		SizingFixedFit = 1u << 13,
		SizingFixedSame = 2u << 13,
		SizingStretchProp = 3u << 13,
		SizingStretchSame = 4u << 13,

		NoHostExtendX = 1u << 16,
		NoHostExtendY = 1u << 17,

		NoKeepColumnsVisible = 1u << 18,
		PreciseWidths = 1u << 19,
		NoClip = 1u << 20,

		PadOuterX = 1u << 21,
		NoPadOuterX = 1u << 22,
		NoPadInnerX = 1u << 23,

		ScrollX = 1u << 24,
		ScrollY = 1u << 25,

		SortMulti = 1u << 26,
		SortTristate = 1u << 27,

		HighlightHoveredColumn = 1u << 28,
	};

	enum class TableColumn : uint32_t
	{
		None = 0,
		Disabled = 1 << 0,
		DefaultHide = 1 << 1,
		DefaultSort = 1 << 2,
		WidthStretch = 1 << 3,
		WidthFixed = 1 << 4,
		NoResize = 1 << 5,
		NoReorder = 1 << 6,
		NoHide = 1 << 7,
		NoClip = 1 << 8,
		NoSort = 1 << 9,
		NoSortAscending = 1 << 10,
		NoSortDescending = 1 << 11,
		NoHeaderLabel = 1 << 12,
		NoHeaderWidth = 1 << 13,
		PreferSortAscending = 1 << 14,
		PreferSortDescending = 1 << 15,
		IndentEnable = 1 << 16,
		IndentDisable = 1 << 17,
		AngledHeader = 1 << 18,
		IsEnabled = 1 << 24,
		IsVisible = 1 << 25,
		IsSorted = 1 << 26,
		IsHovered = 1 << 27,
	};

	enum class TreeNode : uint32_t
	{
		None = 0,
		Selected = 1 << 0,
		Framed = 1 << 1,
		AllowOverlap = 1 << 2,
		NoTreePushOnOpen = 1 << 3,
		NoAutoOpenOnLog = 1 << 4,
		DefaultOpen = 1 << 5,
		OpenOnDoubleClick = 1 << 6,
		OpenOnArrow = 1 << 7,
		Leaf = 1 << 8,
		Bullet = 1 << 9,
		FramePadding = 1 << 10,
		SpanAvailWidth = 1 << 11,
		SpanFullWidth = 1 << 12,
		SpanLabelWidth = 1 << 13,
		SpanAllColumns = 1 << 14,
		LabelSpanAllColumns = 1 << 15,
		NavLeftJumpsToParent = 1 << 17,
		CollapsingHeader = Framed | NoTreePushOnOpen | NoAutoOpenOnLog,
		DrawLinesNone = 1 << 18,
		DrawLinesFull = 1 << 19,
		DrawLinesToNodes = 1 << 20,
	};

	enum class Viewport : uint32_t
	{
		None = 0,
		IsPlatformWindow = 1 << 0,
		IsPlatformMonitor = 1 << 1,
		OwnedByApp = 1 << 2,
		NoDecoration = 1 << 3,
		NoTaskBarIcon = 1 << 4,
		NoFocusOnAppearing = 1 << 5,
		NoFocusOnClick = 1 << 6,
		NoInputs = 1 << 7,
		NoRendererClear = 1 << 8,
		NoAutoMerge = 1 << 9,
		TopMost = 1 << 10,
		CanHostOtherWindows = 1 << 11,

		IsMinimized = 1 << 12,
		IsFocused = 1 << 13,
	};

	enum class Window : uint32_t
	{
		None = 0,

		NoTitleBar = 1u << 0,
		NoResize = 1u << 1,
		NoMove = 1u << 2,
		NoScrollbar = 1u << 3,
		NoScrollWithMouse = 1u << 4,
		NoCollapse = 1u << 5,
		AlwaysAutoResize = 1u << 6,
		NoBackground = 1u << 7,
		NoSavedSettings = 1u << 8,
		NoMouseInputs = 1u << 9,
		MenuBar = 1u << 10,
		HorizontalScrollbar = 1u << 11,
		NoFocusOnAppearing = 1u << 12,
		NoBringToFrontOnFocus = 1u << 13,
		AlwaysVerticalScrollbar = 1u << 14,
		AlwaysHorizontalScrollbar = 1u << 15,

		NoNavInputs = 1u << 16,
		NoNavFocus = 1u << 17,

		UnsavedDocument = 1u << 18,
		NoDocking = 1u << 19,

		NoNav = NoNavInputs | NoNavFocus,
		NoDecoration = NoTitleBar | NoResize | NoScrollbar | NoCollapse,
		NoInputs = NoMouseInputs | NoNavInputs | NoNavFocus,
	};

#pragma region Helpers

	constexpr ColorEdit operator|(ColorEdit a, ColorEdit b)
	{
		return static_cast<ColorEdit>(
			static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	constexpr Combo operator|(Combo a, Combo b)
	{
		return static_cast<Combo>(
			static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	constexpr DockNode operator|(DockNode a, DockNode b)
	{
		return static_cast<DockNode>(
			static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	constexpr DockNode operator|(DockNode a, DockNodeInternal b)
	{
		return static_cast<DockNode>(
			static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	constexpr Hovered operator|(Hovered a, Hovered b)
	{
		return static_cast<Hovered>(
			static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	constexpr InputText operator|(InputText a, InputText b)
	{
		return static_cast<InputText>(
			static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	constexpr Popup operator|(Popup a, Popup b)
	{
		return static_cast<Popup>(
			static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	constexpr Table operator|(Table a, Table b)
	{
		return static_cast<Table>(
			static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	constexpr TreeNode operator|(TreeNode a, TreeNode b)
	{
		return static_cast<TreeNode>(
			static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	constexpr Viewport operator|(Viewport a, Viewport b)
	{
		return static_cast<Viewport>(
			static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	constexpr Window operator|(Window a, Window b)
	{
		return static_cast<Window>(
			static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

#pragma endregion

}
