#include "pch.h"

#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Scope/ListBoxScope.h>

namespace Ludus::UI::Scope
{
	ListBoxScope::ListBoxScope(const char* label, Ludus::Engine::Math::Vector2D size)
	{
		m_Open = ImGui::BeginListBox(label, { size.X, size.Y });
	}

	ListBoxScope::~ListBoxScope()
	{
		if (m_Open)
		{
			ImGui::EndListBox();
		}
	}
}
