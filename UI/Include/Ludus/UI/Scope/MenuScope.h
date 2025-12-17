#pragma once

#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Scope
{
	class MenuScope
	{
	private:
		bool m_Open = false;

	public:
		explicit MenuScope(const char* label, bool enabled = true);

		~MenuScope();

		explicit operator bool() const { return m_Open; }
	};
}
