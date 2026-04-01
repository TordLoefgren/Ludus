#pragma once

#include <string_view>

#include <Ludus/Engine/Graphics/Color.h>

namespace Ludus::UI::Widgets
{
	void TextUnformatted(const char* text);
	void TextUnformatted(std::string_view text);

	void TextUnformattedColor(const char* text, const Ludus::Engine::Graphics::Color& color);
	void TextUnformattedColor(std::string_view text, const Ludus::Engine::Graphics::Color& color);
}
