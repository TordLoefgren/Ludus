#include "pch.h"

#include <Ludus/UI/Scope/MenuScope.h>

namespace Ludus::UI::Scope
{
	MenuScope::MenuScope(const char* label, bool isEnabled)
	{
		m_Open = ImGui::BeginMenu(label, isEnabled);
	}

	MenuScope::~MenuScope()
	{
		if (m_Open)
		{
			ImGui::EndMenu();
		}
	}
}
