#pragma once

#include <cstdint>

#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::UI::Context::WindowContext
{
	enum class WindowCondition : uint8_t
	{
		None = 0,
		Always = 1 << 0,
		Once = 1 << 1,
		FirstUseEver = 1 << 2,
		Appearing = 1 << 3,
	};

	void SetNextWindowPosition(
		const Ludus::Engine::Math::Vector2D& position,
		WindowCondition condition = WindowCondition::None,
		const Ludus::Engine::Math::Vector2D& pivot = { 0.0f, 0.0f }
	);

	void SetNextWindowSize(const Ludus::Engine::Math::Vector2D& size);

	void SetNextWindowViewport(uint32_t viewportId);

	void SetCursorPosition(const Ludus::Engine::Math::Vector2D& position);

	void SetCursorPositionX(float x);

	void SetCursorPositionY(float y);

	Ludus::Engine::Math::Vector2D GetCursorPosition();

	Ludus::Engine::Math::Vector2D GetContentRegionAvailable();

	Ludus::Engine::Math::Vector2D GetWindowContentRegionMin();

	Ludus::Engine::Math::Vector2D GetWindowContentRegionMax();
}
