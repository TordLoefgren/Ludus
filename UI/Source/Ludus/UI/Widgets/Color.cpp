#include "pch.h"

#include <string>

#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Widgets/Color.h>

namespace Ludus::UI::Widgets
{
	bool ColorEdit4(const std::string& label, float color[4], Ludus::UI::Flags::ColorEdit flags)
	{
		return ImGui::ColorEdit4(label.c_str(), color, static_cast<int>(flags));
	}
}
