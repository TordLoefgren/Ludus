#include "pch.h"

#include <Ludus/UI/Scope/IDScope.h>

namespace Ludus::UI::Scope
{
	IDScope::IDScope(const char* id)
	{
		ImGui::PushID(id);
	}

	IDScope::~IDScope()
	{
		ImGui::PopID();
	}
}
