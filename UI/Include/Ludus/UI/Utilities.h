#pragma once

#include <imgui/imgui.h>

namespace Ludus::UI::Utilities
{
	inline void ShowDemoWindow() { ImGui::ShowDemoWindow(); }

	inline ImGuiIO& GetIO() { return ImGui::GetIO(); }
}
