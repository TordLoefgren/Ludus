#include "pch.h"

#include <Ludus/UI/Backend/Fonts.h>

namespace Ludus::UI::Backend
{
	void AddFont(ImGuiIO& io, const char* filepath, float sizePixels)
	{
		io.Fonts->AddFontFromFileTTF(filepath, sizePixels);
	}

	void AddMergedIconFont(ImGuiIO& io, const char* filepath, float sizePixels, float gryphMinAdvanceX, float glyphOffsetY)
	{
		ImFontConfig config { };
		config.MergeMode = true;
		config.GlyphMinAdvanceX = gryphMinAdvanceX;
		config.GlyphOffset = ImVec2(0.0f, glyphOffsetY);

		io.Fonts->AddFontFromFileTTF(filepath, sizePixels, &config);
	}
}
