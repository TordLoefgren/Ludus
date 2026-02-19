#pragma once

#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Scope
{
	constexpr Ludus::UI::Flags::Window DefaultPopupModalFlags = Ludus::UI::Flags::Window::None;

	class PopupModalScope
	{
	private:
		bool m_Open = false;

	public:
		explicit PopupModalScope(const char* label, bool* open, Ludus::UI::Flags::Window flags = DefaultPopupModalFlags);

		~PopupModalScope();

		explicit operator bool() const { return m_Open; }
	};
}
