#include "pch.h"

#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Scope/TableScope.h>

namespace Ludus::UI::Scope
{
	TableScope::TableScope(const char* label, int columns, Ludus::UI::Flags::Table flags)
	{
		m_Open = ImGui::BeginTable(label, columns, static_cast<int>(flags));
	}

	TableScope::~TableScope()
	{
		if (m_Open)
		{
			ImGui::EndTable();
		}
	}
}
