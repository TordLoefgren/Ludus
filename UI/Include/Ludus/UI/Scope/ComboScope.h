#pragma once

#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Scope
{
	constexpr Ludus::UI::Flags::Combo DefaultComboFlags = Ludus::UI::Flags::Combo::None;

	class ComboScope
	{
	private:
		bool m_Open = false;

	public:
		explicit ComboScope(const char* label, const char* previewValue, Ludus::UI::Flags::Combo flags = DefaultComboFlags);

		~ComboScope();

		explicit operator bool() const { return m_Open; }
	};
}
