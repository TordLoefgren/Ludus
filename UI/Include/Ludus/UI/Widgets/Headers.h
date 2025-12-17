#pragma once

#include <string>

#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Widgets
{
	bool CollapsingHeader(const std::string& label, Ludus::UI::Flags::TreeNode flags = Ludus::UI::Flags::TreeNode::None);
}
