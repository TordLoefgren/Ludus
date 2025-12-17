#include "pch.h"

#include <cstdint>

#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Context/DockingContext.h>
#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Context::DockingContext
{
	uint32_t CreateDockSpace(
		const char* label,
		Ludus::Engine::Math::Vector2D size,
		Ludus::UI::Flags::DockNode flags
	)
	{
		auto dockspaceId = ImGui::GetID(label);
		return ImGui::DockSpace(dockspaceId, { size.X, size.Y }, static_cast<int>(flags));
	}
}
