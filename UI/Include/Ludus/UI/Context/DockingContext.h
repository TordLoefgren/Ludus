#pragma once

#include <cstdint>

#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Context::DockingContext
{
	uint32_t CreateDockSpace(
		const char* label,
		Ludus::Engine::Math::Vector2D size = { 0.0f, 0.0f },
		Ludus::UI::Flags::DockNode flags = Ludus::UI::Flags::DockNode::None
	);
}
