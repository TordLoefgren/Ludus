#include "pch.h"

#include <Ludus/UI/Context/PopupContext.h>

namespace Ludus::UI::Context::PopupContext
{
	void CloseCurrentPopup() { ImGui::CloseCurrentPopup(); }

	void OpenPopup(const char* id, Ludus::UI::Flags::Popup flags)
	{
		ImGui::OpenPopup(id, static_cast<int>(flags));
	}
}
