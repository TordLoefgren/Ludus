#pragma once

#include <string_view>

namespace Ludus::UI::Widgets
{
	void TextUnformatted(const char* text);

	void TextUnformatted(std::string_view text);
}
