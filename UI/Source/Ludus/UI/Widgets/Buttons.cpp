#include "pch.h"

#include <Ludus/UI/Flags/Flags.h>
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

	bool InvisibleButton(const char* label)
	{
		return ImGui::InvisibleButton(label, { 0.0f, 0.0f });
	}

	bool InvisibleButton(const char* label, Ludus::Engine::Math::Vector2D size, Ludus::UI::Flags::Button flags)
	{
		return ImGui::InvisibleButton(label, { size.X, size.Y }, static_cast<int>(flags));
	}
}
