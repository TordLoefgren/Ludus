#include "pch.h"

#include <Ludus/UI/Scope/MenuBarScope.h>

namespace Ludus::UI::Scope
{
	MenuBarScope::MenuBarScope()
	{
		m_Open = ImGui::BeginMenuBar();
	}

	MenuBarScope::~MenuBarScope()
	{
		if (m_Open)
		{
			ImGui::EndMenuBar();
		}
	}
}
