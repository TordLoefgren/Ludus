#pragma once

#include <string>

#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::UI::Context::LayoutContext
{
	void AlignTextToFramePadding();

	void SameLine(float offsetFromStart = 0.0f, float spacing = 0.0f);

	void SetNextItemWidth(float width);
	void SetNextItemWidthFill();

	void Separator();

	float CalculateItemWidth();

	Ludus::Engine::Math::Vector2D GetItemRectMin();
	Ludus::Engine::Math::Vector2D GetItemRectMax();

	float GetFrameHeight();

	Ludus::Engine::Math::Vector2D CalculateTextSize(std::string text);

	void DrawRect(
		const Ludus::Engine::Math::Vector2D& position,
		const Ludus::Engine::Math::Vector2D& size,
		const Ludus::Engine::Graphics::Color color
	);
}
