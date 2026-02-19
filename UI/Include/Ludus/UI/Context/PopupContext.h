#pragma once

#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Context::PopupContext
{
	constexpr Ludus::UI::Flags::Popup DefaultPopupContextItemFlags = Ludus::UI::Flags::Popup::None;

	void OpenPopup(const char* id, Ludus::UI::Flags::Popup flags = DefaultPopupContextItemFlags);
	void CloseCurrentPopup();
}
