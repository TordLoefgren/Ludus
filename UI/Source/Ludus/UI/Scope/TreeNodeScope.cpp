#include "pch.h"

#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Scope/TreeNodeScope.h>

namespace Ludus::UI::Scope
{
	TreeNodeScope::TreeNodeScope(const char* label, Ludus::UI::Flags::TreeNode flags)
	{
		m_Open = ImGui::TreeNodeEx(label, static_cast<int>(flags));
	}

	TreeNodeScope::~TreeNodeScope()
	{
		if (m_Open)
		{
			ImGui::TreePop();
		}
	}
}
