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

	void SetCursorPosition(Ludus::Engine::Math::Vector2D position)
	{
		ImGui::SetCursorPos({ position.X, position.Y });
	}

	void SetCursorPositionX(float x)
	{
		ImGui::SetCursorPosX(x);
	}

	void SetCursorPositionY(float y)
	{
		ImGui::SetCursorPosX(y);
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

	Ludus::Engine::Math::Vector2D GetWindowContentRegionMin()
	{
		const auto contentRegionMin = ImGui::GetWindowContentRegionMin();
		return { contentRegionMin.x, contentRegionMin.y };
	}

	Ludus::Engine::Math::Vector2D GetWindowContentRegionMax()
	{
		const auto contentRegionMax = ImGui::GetWindowContentRegionMax();
		return { contentRegionMax.x, contentRegionMax.y };
	}
}
