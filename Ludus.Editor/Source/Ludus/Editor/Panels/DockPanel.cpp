#include "Ludus/Editor/Panels/DockPanel.h"

namespace Ludus::Editor::Panels
{
	void DockPanel::DrawPanel()
	{
		const auto* viewport = ImGui::GetMainViewport();

		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		if (Ludus::UI::Containers::Window window("DockPanelWindow", nullptr, Constants::DockPanelWindowFlags); window)
		{
			auto dockspaceId = ImGui::GetID("DockPanel");
			ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
		}

		ImGui::PopStyleVar(2);
	}
}
