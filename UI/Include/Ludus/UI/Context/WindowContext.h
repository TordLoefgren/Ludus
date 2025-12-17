#pragma once

#include <cstdint>

#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::UI::Context::WindowContext
{
	void SetNextWindowPosition(Ludus::Engine::Math::Vector2D position);

	void SetNextWindowSize(Ludus::Engine::Math::Vector2D size);

	void SetNextWindowViewport(uint32_t viewportId);

	void SetCursorPosition(Ludus::Engine::Math::Vector2D position);

	Ludus::Engine::Math::Vector2D GetCursorPosition();

	Ludus::Engine::Math::Vector2D GetContentRegionAvailable();
}
