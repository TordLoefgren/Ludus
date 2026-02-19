#include "pch.h"

#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Scope/ModalScope.h>

namespace Ludus::UI::Scope
{
	PopupModalScope::PopupModalScope(const char* label, bool* open, Ludus::UI::Flags::Window flags)
	{
		m_Open = ImGui::BeginPopupModal(label, open, static_cast<int>(flags));
	}

	PopupModalScope::~PopupModalScope()
	{
		if (m_Open)
		{
			ImGui::EndPopup();
		}
	}
}
