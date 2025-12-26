#include "pch.h"

#include <Ludus/Editor/Panels/ImGuiDemoPanel.h>
#include <Ludus/UI/Widgets/Demo.h>

namespace Ludus::Editor::Panels
{
	bool ImGuiDemoPanel::UpdateImpl(Ludus::Editor::Panels::PanelContext& context)
	{
		Ludus::UI::Widgets::ShowDemoWindow(&m_Open);

		return true;
	}
}
