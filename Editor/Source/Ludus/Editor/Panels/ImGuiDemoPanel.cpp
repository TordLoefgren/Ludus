#include "pch.h"

#include <Ludus/Editor/Panels/ImGuiDemoPanel.h>
#include <Ludus/UI/Utilities.h>

namespace Ludus::Editor::Panels
{
	void ImGuiDemoPanel::UpdateImpl(Ludus::Editor::Panels::PanelContext& context)
	{
		Ludus::UI::Utilities::ShowDemoWindow();
	}
}
