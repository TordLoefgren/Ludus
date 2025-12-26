#pragma once

#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Scope
{
	class ComboScope
	{
	private:
		bool m_Open = false;

	public:
		explicit ComboScope(const char* label, const char* previewValue, Ludus::UI::Flags::Combo flags = Ludus::UI::Flags::Combo::None);

		~ComboScope();

		explicit operator bool() const { return m_Open; }
	};
}
