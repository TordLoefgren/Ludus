#pragma once

#include <cstdint>

#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Types
{
	struct Viewport
	{
		uint32_t Id;
		Ludus::UI::Flags::Viewport Flags;
		Ludus::Engine::Math::Vector2D Position;
		Ludus::Engine::Math::Vector2D Size;
		Ludus::Engine::Math::Vector2D FramebufferScale;
		Ludus::Engine::Math::Vector2D WorkPosition;
		Ludus::Engine::Math::Vector2D WorkSize;
		float DpiScale;
	};
}
