#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

namespace Ludus::UI::Layouts
{
	inline void TableProperty(const char* label, auto drawCallback)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);

		drawCallback();
	}

	inline void TableSetupColumn(const char* label, ImGuiTableColumnFlags flags, float initWidthOrWeight)
	{
		ImGui::TableSetupColumn(label, flags, initWidthOrWeight);
	}
}
