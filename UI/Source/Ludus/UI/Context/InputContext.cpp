#include "pch.h"

#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/Engine/Windowing/MouseButton.h>
#include <Ludus/UI/Context/InputContext.h>
#include <Ludus/UI/Flags/Flags.h>
#include <stdexcept>

namespace
{
	int ToImGuiMouseButton(Ludus::Engine::Windowing::MouseButton mouseButton)
	{
		switch (mouseButton)
		{
		case Ludus::Engine::Windowing::MouseButton::Left:   return ImGuiMouseButton_Left;
		case Ludus::Engine::Windowing::MouseButton::Right:  return ImGuiMouseButton_Right;
		case Ludus::Engine::Windowing::MouseButton::Middle: return ImGuiMouseButton_Middle;
		default:
			throw std::runtime_error("Unexpected MouseButton value.");
		}
	}
}

namespace Ludus::UI::Context::InputContext
{
	Ludus::Engine::Math::Vector2D MouseDelta()
	{
		const auto mouseDelta = ImGui::GetIO().MouseDelta;
		return  { mouseDelta.x, mouseDelta.y };
	}

	Ludus::Engine::Math::Vector2D MousePosition()
	{
		const auto mousePosition = ImGui::GetIO().MousePos;
		return  { mousePosition.x, mousePosition.y };
	}

	bool WantCaptureMouse() { return ImGui::GetIO().WantCaptureMouse; }
	bool WantCaptureKeyboard() { return ImGui::GetIO().WantCaptureKeyboard; }

	bool IsAnyItemHovered() { return ImGui::IsAnyItemHovered(); }
	bool IsAnyItemActive() { return ImGui::IsAnyItemActive(); }
	bool IsItemHovered() { return ImGui::IsItemHovered(); }
	bool IsMouseClicked(Ludus::Engine::Windowing::MouseButton mouseButton) { return ImGui::IsMouseClicked(ToImGuiMouseButton(mouseButton)); }
	bool IsWindowHovered(Ludus::UI::Flags::Hovered flags) { return ImGui::IsWindowHovered(static_cast<ImGuiHoveredFlags>(flags)); }
}
