#pragma once

#include <string>

#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Widgets
{
	bool ColorEdit4(const std::string& label, float color[4], Ludus::UI::Flags::ColorEdit flags = Ludus::UI::Flags::ColorEdit::Float | Ludus::UI::Flags::ColorEdit::NoInputs);
}
