#include "pch.h"

#include <Ludus/Engine/Graphics/Color.h>
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

	void TextUnformattedColor(const char* text, const Ludus::Engine::Graphics::Color& color)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, { color.R, color.G, color.B, color.A });
		ImGui::TextUnformatted(text);
		ImGui::PopStyleColor();
	}

	void TextUnformattedColor(std::string_view text, const Ludus::Engine::Graphics::Color& color)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, { color.R, color.G, color.B, color.A });
		ImGui::TextUnformatted(text.data(), text.data() + text.size());
		ImGui::PopStyleColor();
	}
}
