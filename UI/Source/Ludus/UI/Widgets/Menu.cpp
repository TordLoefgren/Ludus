#include "pch.h"

#include <Ludus/UI/Widgets/Menu.h>

namespace Ludus::UI::Widgets
{
	bool MenuItem(const char* label, const char* shortcut, bool selected, bool enabled) { return ImGui::MenuItem(label, shortcut, selected, enabled); }
}
