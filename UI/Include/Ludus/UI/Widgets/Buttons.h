#pragma once

#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::UI::Widgets
{
	bool Button(const char* label);

	bool Button(const char* label, Ludus::Engine::Math::Vector2D size);
}
