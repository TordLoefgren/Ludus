#include "pch.h"

#include <Ludus/Editor/Panels/SceneHierarchyPanel.h>

namespace Ludus::Editor::Panels
{
	void SceneHierarchyPanel::DrawPanel()
	{
		static auto open = true;
		if (Ludus::UI::Containers::Window window("Scene Hierarchy", &open, Constants::PanelFlags); window)
		{

		}
	}
}
