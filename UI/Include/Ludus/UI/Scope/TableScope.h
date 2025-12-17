#pragma once

#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Scope
{
	constexpr Ludus::UI::Flags::Table DefaultTableFlags = Ludus::UI::Flags::Table::None;

	class TableScope
	{
	private:
		bool m_Open = false;

	public:
		explicit TableScope(const char* label, int columns, Ludus::UI::Flags::Table flags = DefaultTableFlags);

		~TableScope();

		explicit operator bool() const { return m_Open; }
	};
}
