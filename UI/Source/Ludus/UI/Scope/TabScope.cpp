#include "pch.h"

#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Scope/TabScope.h>

namespace Ludus::UI::Scope
{
	TabBarScope::TabBarScope(const char* label, Ludus::UI::Flags::TabBar flags)
	{
		m_Open = ImGui::BeginTabBar(label, static_cast<int>(flags));
	}

	TabBarScope::~TabBarScope()
	{
		if (m_Open)
		{
			ImGui::EndTabBar();
		}
	}

	TabItemScope::TabItemScope(const char* label, bool* open, Ludus::UI::Flags::TabItem flags)
	{
		m_Open = ImGui::BeginTabItem(label, open, static_cast<int>(flags));
	}

	TabItemScope::~TabItemScope()
	{
		if (m_Open)
		{
			ImGui::EndTabItem();
		}
	}
}
