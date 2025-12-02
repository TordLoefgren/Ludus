#pragma once

#include <imgui/imgui.h>

namespace Ludus::Editor::Panels::Constants
{
	inline ImGuiWindowFlags PanelFlags = ImGuiBackendFlags_None;
	inline ImGuiWindowFlags DockPanelWindowFlags = ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_MenuBar
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_NoNavFocus;
}
