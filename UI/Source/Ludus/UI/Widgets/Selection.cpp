#include "pch.h"

#include <Ludus/UI/Widgets/Selection.h>

namespace Ludus::UI::Widgets
{
	bool Selectable(const char* label, bool selected) { return ImGui::Selectable(label, selected); }

	bool Combo(const std::string& label, int* currentItem, std::vector<const char*> items)
	{
		return ImGui::Combo(label.c_str(), currentItem, items.data(), static_cast<int>(items.size()));
	}
}
