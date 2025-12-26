#pragma once

struct ImGuiIO;

namespace Ludus::UI::Backend
{
	void AddFont(ImGuiIO& io, const char* filepath, float sizePixels);

	void AddMergedIconFont(ImGuiIO& io, const char* filepath, float sizePixels, float gryphMinAdvanceX = 0.0f);
}
