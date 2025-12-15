#include "pch.h"

#include <imgui/imgui.h>

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Panels/DockPanel.h>

namespace Ludus::Editor::Panels
{
	void DockPanel::UpdateImpl(Ludus::Editor::Panels::PanelContext& context)
	{
		const auto* viewport = ImGui::GetMainViewport();

		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		// The dock panel should never close, as it enables docking for all other panels.
		const auto flags = Ludus::Editor::Core::Constants::PanelFlags | ImGuiWindowFlags_HorizontalScrollbar;
		auto windowTitle = CreateWindowTitle("DockPanel");

		if (Ludus::UI::Containers::Window window(windowTitle.c_str(), nullptr, Ludus::Editor::Core::Constants::DockPanelWindowFlags); window)
		{
			auto dockspaceId = ImGui::GetID(windowTitle.c_str());
			ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
		}

		ImGui::PopStyleVar(2);
	}
}
