#pragma once

#include "pch.h"

namespace Ludus::UI::Layout
{
	void TablePropertyRow(const char* label, auto drawCallback)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);

		drawCallback();
	}
}
