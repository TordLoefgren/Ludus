#include "pch.h"

#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Scope/ComboScope.h>

namespace Ludus::UI::Scope
{
	ComboScope::ComboScope(const char* label, const char* previewValue, Ludus::UI::Flags::Combo flags)
	{
		m_Open = ImGui::BeginCombo(label, previewValue, static_cast<int>(flags));
	}

	ComboScope::~ComboScope()
	{
		if (m_Open)
		{
			ImGui::EndCombo();
		}
	}
}
