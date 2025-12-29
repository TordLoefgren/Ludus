#include "pch.h"

#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Scope/PopupScope.h>

namespace Ludus::UI::Scope
{
	void OpenPopup(const char* id, Ludus::UI::Flags::Popup flags)
	{
		ImGui::OpenPopup(id, static_cast<int>(flags));
	}

	PopupScope::PopupScope(const char* id, Ludus::UI::Flags::Window flags)
	{
		m_Open = ImGui::BeginPopup(id, static_cast<int>(flags));
	}

	PopupScope::~PopupScope()
	{
		if (m_Open)
		{
			ImGui::EndPopup();
		}
	}

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
