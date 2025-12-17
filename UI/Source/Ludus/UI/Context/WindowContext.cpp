#include "pch.h"

#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Context/WindowContext.h>

namespace Ludus::UI::Context::WindowContext
{
	void SetNextWindowPosition(Ludus::Engine::Math::Vector2D position)
	{
		ImGui::SetNextWindowPos({ position.X, position.Y });
	}

	void SetNextWindowSize(Ludus::Engine::Math::Vector2D size)
	{
		ImGui::SetNextWindowSize({ size.X, size.Y });
	}

	void SetNextWindowViewport(uint32_t viewportId)
	{
		ImGui::SetNextWindowViewport(viewportId);
	}

	Ludus::Engine::Math::Vector2D GetCursorPosition()
	{
		const auto cursorPosition = ImGui::GetCursorPos();
		return { cursorPosition.x, cursorPosition.y };
	}

	Ludus::Engine::Math::Vector2D GetContentRegionAvailable()
	{
		const auto availableSpace = ImGui::GetContentRegionAvail();
		return { availableSpace.x, availableSpace.y };
	}

	void SetCursorPosition(Ludus::Engine::Math::Vector2D position)
	{
		ImGui::SetCursorPos({ position.X, position.Y });
	}
}
