#pragma once

namespace Ludus::UI::Context::LayoutContext
{
	void SameLine(float offsetFromStart = 0.0f, float spacing = 0.0f);

	void SetNextItemWidth(float width);

	void Separator();
}
