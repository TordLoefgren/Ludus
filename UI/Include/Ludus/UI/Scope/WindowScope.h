#pragma once

#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Scope
{
	constexpr Ludus::UI::Flags::Window DefaultWindowFlags = Ludus::UI::Flags::Window::None;

	class WindowScope
	{
	private:
		bool m_Active = false;

	public:
		explicit WindowScope(const char* name, bool* open, Ludus::UI::Flags::Window flags = DefaultWindowFlags);

		~WindowScope();

		explicit operator bool() const { return m_Active; }
	};
}
