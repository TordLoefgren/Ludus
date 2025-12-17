#pragma once

#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/Engine/Platform/MouseButton.h>
#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Context::InputContext
{
	Ludus::Engine::Math::Vector2D MouseDelta();
	Ludus::Engine::Math::Vector2D MousePosition();

	bool WantCaptureMouse();
	bool WantCaptureKeyboard();

	bool IsMouseClicked(Ludus::Engine::Platform::MouseButton mouseButton);
	bool IsWindowHovered(Ludus::UI::Flags::Hovered flags = Ludus::UI::Flags::Hovered::None);
	bool IsAnyItemHovered();
}
