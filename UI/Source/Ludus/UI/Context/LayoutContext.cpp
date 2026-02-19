#include "pch.h"

#include <Ludus/UI/Context/LayoutContext.h>

#pragma once

namespace Ludus::UI::Context::LayoutContext
{
	void AlignTextToFramePadding()
	{
		ImGui::AlignTextToFramePadding();
	}

	void SameLine(float offsetFromStart, float spacing)
	{
		ImGui::SameLine(offsetFromStart, spacing);
	}

	void SetNextItemWidth(float width)
	{
		ImGui::SetNextItemWidth(width);
	}

	void Separator()
	{
		ImGui::Separator();
	}
}
