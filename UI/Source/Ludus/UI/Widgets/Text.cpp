#include "pch.h"

#include <Ludus/UI/Widgets/Text.h>

namespace Ludus::UI::Widgets
{
	void TextUnformatted(const char* text)
	{
		ImGui::TextUnformatted(text);
	}

	void TextUnformatted(std::string_view text)
	{
		ImGui::TextUnformatted(text.data(), text.data() + text.size());
	}
}
