#pragma once

#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Widgets
{
	bool Button(const char* label);
	bool Button(const char* label, Ludus::Engine::Math::Vector2D size);

	bool InvisibleButton(const char* label);
	bool InvisibleButton(const char* label, Ludus::Engine::Math::Vector2D size, Ludus::UI::Flags::Button flags = Ludus::UI::Flags::Button::None);
}
