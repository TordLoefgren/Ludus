#include "pch.h"

#include <Ludus/Editor/Panels/SceneHierarchyPanel.h>

namespace Ludus::Editor::Panels
{
	void SceneHierarchyPanel::DrawPanel()
	{
		static bool open = true;
		if (Ludus::UI::Containers::Window window("Scene Hierarchy", &open, Constants::PanelFlags); window)
		{

		}
	}
}
