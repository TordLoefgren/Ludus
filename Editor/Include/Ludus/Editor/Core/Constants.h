#pragma once

#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::Editor::Core::Constants
{
	inline Ludus::UI::Flags::Window PanelFlags = Ludus::UI::Flags::Window::None;

	inline Ludus::UI::Flags::Window DockPanelWindowFlags = Ludus::UI::Flags::Window::NoDocking
		| Ludus::UI::Flags::Window::MenuBar
		| Ludus::UI::Flags::Window::NoTitleBar
		| Ludus::UI::Flags::Window::NoCollapse
		| Ludus::UI::Flags::Window::NoResize
		| Ludus::UI::Flags::Window::NoMove
		| Ludus::UI::Flags::Window::NoBringToFrontOnFocus
		| Ludus::UI::Flags::Window::NoNavFocus;
}
