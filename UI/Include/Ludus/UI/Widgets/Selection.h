#pragma once

#include <string>
#include <vector>

namespace Ludus::UI::Widgets
{
	bool Selectable(const char* label, bool selected = false);

	bool Combo(const std::string& label, int* currentItem, std::vector<const char*> items);
}
