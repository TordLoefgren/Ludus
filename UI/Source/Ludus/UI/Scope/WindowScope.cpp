#include "pch.h"

#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Scope/WindowScope.h>

namespace Ludus::UI::Scope
{
	WindowScope::WindowScope(const char* name, bool* open, Ludus::UI::Flags::Window flags)
	{
		m_Active = ImGui::Begin(name, open, static_cast<int>(flags));
	}

	WindowScope::~WindowScope()
	{
		ImGui::End();
	}
}
