#include "pch.h"

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Panels/HierarchyPanel.h>
#include <Ludus/UI/Containers.h>

namespace Ludus::Editor::Panels
{
	void HierarchyPanel::UpdateImpl(Ludus::Editor::Panels::PanelContext& context)
	{
		auto windowTitle = CreateWindowTitle("Hierarchy");
		if (Ludus::UI::Containers::Window window(windowTitle.c_str(), &m_Open, Ludus::Editor::Core::Constants::PanelFlags); window)
		{

		}
	}
}
