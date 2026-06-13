#pragma once

#include <Ludus/Engine/Core/Version.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::Editor::Core::Constants
{
	inline constexpr Ludus::Engine::Core::Version CurrentVersion { 0, 3, 0 };

	namespace Shared
	{
		inline constexpr float StandardInlineSpacing = 6.0f;
		inline const Ludus::Engine::Math::Vector2D ModalActionButtonSize = { 120.0f, 0.0f };
		inline constexpr float ToolbarButtonExtent = 30.0f;
	}

	namespace Flags
	{
		inline constexpr Ludus::UI::Flags::Window Panel = Ludus::UI::Flags::Window::None;

		inline constexpr Ludus::UI::Flags::Window DockPanelWindow = Ludus::UI::Flags::Window::NoDocking
			| Ludus::UI::Flags::Window::MenuBar
			| Ludus::UI::Flags::Window::NoBringToFrontOnFocus
			| Ludus::UI::Flags::Window::NoDecoration
			| Ludus::UI::Flags::Window::NoMove
			| Ludus::UI::Flags::Window::NoNav;

		inline constexpr Ludus::UI::Flags::Window WelcomeWindow = Ludus::UI::Flags::Window::NoDocking
			| Ludus::UI::Flags::Window::NoDecoration
			| Ludus::UI::Flags::Window::NoMove
			| Ludus::UI::Flags::Window::NoNav
			| Ludus::UI::Flags::Window::NoNavFocus;

		inline constexpr Ludus::UI::Flags::Window WelcomeBackgroundWindow = WelcomeWindow
			| Ludus::UI::Flags::Window::NoBringToFrontOnFocus
			| Ludus::UI::Flags::Window::NoMouseInputs
			| Ludus::UI::Flags::Window::NoSavedSettings;
	}

	namespace Welcome
	{
		namespace Layout
		{
			inline const Ludus::Engine::Math::Vector2D WindowSize = { 680.0f, 520.0f };
			inline const Ludus::Engine::Math::Vector2D ActionButtonSize = { 160.0f, 44.0f };

			inline const float WindowBorderSize = 1.0f;
			inline const float WindowRounding = 6.0f;
			inline const float TitleFontSize = 42.0f;

			inline const float TitleY = 28.0f;
			inline const float SubtitleY = 84.0f;
			inline const float ActionButtonsY = 138.0f;
			inline const float RecentProjectsLabelY = 208.0f;
			inline const float RecentListY = 248.0f;
			inline const float ErrorBottomOffset = 56.0f;
			inline const float FooterBottomOffset = 28.0f;

			inline const float ActionButtonsSpacing = Shared::StandardInlineSpacing * 5.0f;
		}

		namespace RecentProjects
		{
			inline const int VisibleItems = 3;
			inline const float ListExtraWidth = 96.0f;
			inline const float ListPadding = 10.0f;
			inline const float ListBorderSize = 1.0f;
			inline const float ListRounding = 6.0f;
			inline const float ListEmptyStateY = 28.0f;

			inline float CalculateListWidth(float actionButtonWidth)
			{
				return actionButtonWidth * 2.0f + Layout::ActionButtonsSpacing + ListExtraWidth;
			}

			inline float CalculateListHeight()
			{
				return ListPadding * 2.0f + VisibleItems * 52.0f + (VisibleItems - 1) * ListPadding;
			}
		}

		namespace RecentProjectRow
		{
			inline const float Height = 52.0f;
			inline const float Spacing = RecentProjects::ListPadding;
			inline const float Rounding = 6.0f;
			inline const float TextPaddingX = 12.0f;
			inline const float TitleOffsetY = 8.0f;
			inline const float PathOffsetY = 28.0f;
			inline const float BadgeGap = 12.0f;
		}
	}
}
