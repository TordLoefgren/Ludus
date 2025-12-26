#include "pch.h"

#include <Ludus/UI/Widgets/Buttons.h>

namespace Ludus::UI::Widgets
{
	bool Button(const char* label)
	{
		return ImGui::Button(label);
	}

	bool Button(const char* label, Ludus::Engine::Math::Vector2D size)
	{
		return ImGui::Button(label, { size.X, size.Y });
	}
}
