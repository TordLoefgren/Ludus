#include "pch.h"

#include <cstdint>
#include <string>

#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Widgets/Headers.h>

namespace Ludus::UI::Widgets
{
	bool CollapsingHeader(const std::string& label, Ludus::UI::Flags::TreeNode flags)
	{
		return ImGui::CollapsingHeader(label.c_str(), static_cast<int>(flags));
	}
}
