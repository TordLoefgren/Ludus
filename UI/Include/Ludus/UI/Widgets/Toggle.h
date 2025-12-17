#pragma once

#include <string>

namespace Ludus::UI::Widgets
{
	struct CheckboxItem
	{
		std::string Label;
		bool IsChecked = false;
	};

	bool Checkbox(CheckboxItem& item);

	bool Checkbox(const std::string& label, bool* value);
}
