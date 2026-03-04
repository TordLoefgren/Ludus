#include "pch.h"

#include <Ludus/UI/Scope/FontScope.h>

namespace Ludus::UI::Scope
{
	FontScope::FontScope(float size)
	{
		ImGui::PushFont(nullptr, size);
	}

	FontScope::~FontScope()
	{
		ImGui::PopFont();
	}
}
