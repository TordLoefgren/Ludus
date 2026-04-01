#pragma once

#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::Editor::Core::Constants
{
	inline Ludus::UI::Flags::Window PanelFlags = Ludus::UI::Flags::Window::None;
	inline float StandardInlineSpacing = 6.0f;
	inline Ludus::Engine::Math::Vector2D ModalActionButtonSize = { 120.0f, 0.0f };

	inline Ludus::UI::Flags::Window DockPanelWindowFlags = Ludus::UI::Flags::Window::NoDocking
		| Ludus::UI::Flags::Window::MenuBar
		| Ludus::UI::Flags::Window::NoBringToFrontOnFocus
		| Ludus::UI::Flags::Window::NoDecoration
		| Ludus::UI::Flags::Window::NoMove
		| Ludus::UI::Flags::Window::NoNav;

	inline Ludus::UI::Flags::Window WelcomeWindowFlags = Ludus::UI::Flags::Window::NoDocking
		| Ludus::UI::Flags::Window::NoDecoration
		| Ludus::UI::Flags::Window::NoMove
		| Ludus::UI::Flags::Window::NoNav
		| Ludus::UI::Flags::Window::NoNavFocus;

	inline Ludus::UI::Flags::Window WelcomeBackgroundWindowFlags = WelcomeWindowFlags
		| Ludus::UI::Flags::Window::NoBringToFrontOnFocus
		| Ludus::UI::Flags::Window::NoMouseInputs
		| Ludus::UI::Flags::Window::NoSavedSettings;

	inline Ludus::Engine::Math::Vector2D WelcomeWindowSize = { 420.0f, 300.0f };
	inline Ludus::Engine::Math::Vector2D WelcomeActionButtonSize = { 140.0f, 42.0f };

	inline float ToolbarButtonExtent = 30.0f;
}
