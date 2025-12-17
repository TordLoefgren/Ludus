#include "pch.h"

#include <string>

#include <Ludus/UI/Widgets/Toggle.h>

namespace Ludus::UI::Widgets
{
	bool Checkbox(CheckboxItem& item)
	{
		return ImGui::Checkbox(item.Label.c_str(), &item.IsChecked);
	}

	bool Checkbox(const std::string& label, bool* value)
	{
		return ImGui::Checkbox(label.c_str(), value);
	}
}
