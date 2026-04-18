#include "pch.h"

#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Scope/ChildScope.h>

namespace Ludus::UI::Scope
{
	ChildScope::ChildScope(
		const char* id,
		const Ludus::Engine::Math::Vector2D size,
		Ludus::UI::Flags::Child childFlags,
		Ludus::UI::Flags::Window windowFlags
	)
	{
		m_Active = ImGui::BeginChild(
			id,
			{ size.X, size.Y },
			static_cast<int>(childFlags),
			static_cast<int>(windowFlags)
		);
	}

	ChildScope::~ChildScope()
	{
		ImGui::EndChild();
	}
}
