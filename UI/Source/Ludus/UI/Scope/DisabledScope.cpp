#include "pch.h"

#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Scope/DisabledScope.h>

namespace Ludus::UI::Scope
{
	DisabledScope::DisabledScope(bool disabled)
		: m_Active(disabled)
	{
		if (m_Active)
		{
			ImGui::BeginDisabled();
		}
	}

	DisabledScope::~DisabledScope()
	{
		if (m_Active)
		{
			ImGui::EndDisabled();
		}
	}
}
