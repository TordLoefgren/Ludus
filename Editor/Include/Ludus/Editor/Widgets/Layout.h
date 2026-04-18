#pragma once

#include <string_view>

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Context/LayoutContext.h>

namespace Ludus::Editor::Widgets::Layout
{
	inline Ludus::Engine::Math::Vector2D GetCenteredTextPositionAtY(std::string_view text, float y, float containerWidth)
	{
		const auto textSize = Ludus::UI::Context::LayoutContext::CalculateTextSize(std::string(text));

		return {
			(containerWidth - textSize.X) * 0.5f,
			y
		};
	}

	inline Ludus::Engine::Math::Vector2D GetCenteredRowElementPositionAtY(
		float y,
		float containerWidth,
		float elementWidth,
		float spacing,
		int elementCount,
		int elementIndex
	)
	{
		LUDUS_ASSERT(elementCount > 0, "The element count in GetCenteredRowElementPositionAtY must be above zero.");
		LUDUS_ASSERT(elementIndex >= 0, "The element index in GetCenteredRowElementPositionAtY must be non-negative.");
		LUDUS_ASSERT(elementIndex < elementCount, "The element index in GetCenteredRowElementPositionAtY must be within range.");

		const auto totalWidth = elementCount * elementWidth + (elementCount - 1) * spacing;
		const auto offset = elementIndex * (elementWidth + spacing);

		return {
			(containerWidth - totalWidth) * 0.5f + offset,
			y
		};
	}
}
