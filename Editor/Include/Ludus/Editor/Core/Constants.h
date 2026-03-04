#pragma once

#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::Editor::Core::Constants
{
	inline Ludus::UI::Flags::Window PanelFlags = Ludus::UI::Flags::Window::None;

	inline Ludus::UI::Flags::Window DockPanelWindowFlags = Ludus::UI::Flags::Window::NoDocking
		| Ludus::UI::Flags::Window::MenuBar
		| Ludus::UI::Flags::Window::NoBringToFrontOnFocus
		| Ludus::UI::Flags::Window::NoDecoration
		| Ludus::UI::Flags::Window::NoMove
		| Ludus::UI::Flags::Window::NoNav;

	inline Ludus::UI::Flags::Window WelcomePanelWindowFlags = Ludus::UI::Flags::Window::NoDocking
		| Ludus::UI::Flags::Window::NoDecoration
		| Ludus::UI::Flags::Window::NoMove
		| Ludus::UI::Flags::Window::NoNav
		| Ludus::UI::Flags::Window::NoNavFocus;
}
