#include "pch.h"

#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Scope/PopupScope.h>

namespace Ludus::UI::Scope
{
	PopupContextItemScope::PopupContextItemScope(const char* label, Ludus::UI::Flags::Popup flags)
	{
		m_Open = ImGui::BeginPopupContextItem(label, static_cast<int>(flags));
	}

	PopupContextItemScope::~PopupContextItemScope()
	{
		if (m_Open)
		{
			ImGui::EndPopup();
		}
	}

	PopupContextWindowScope::PopupContextWindowScope(const char* id, Ludus::UI::Flags::Popup flags)
	{
		m_Active = ImGui::BeginPopupContextWindow(id, static_cast<int>(flags));
	}

	PopupContextWindowScope::~PopupContextWindowScope()
	{
		if (m_Active)
		{
			ImGui::EndPopup();
		}
	}
}
