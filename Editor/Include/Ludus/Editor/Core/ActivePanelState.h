#pragma once

#include <Ludus/Editor/Panels/PanelKind.h>

namespace Ludus::Editor::Core
{
	struct ActivePanelState
	{
		bool ShowConsolePanel = true;
		bool ShowContentPanel = true;
		bool ShowHierarchyPanel = true;
		bool ShowInspectorPanel = true;
		bool ShowImGuiDemoPanel = true;

		bool IsVisible(const Ludus::Editor::Panels::PanelKind panelKind) const
		{
			switch (panelKind)
			{
				case Ludus::Editor::Panels::PanelKind::Console:
					return ShowConsolePanel;
				case Ludus::Editor::Panels::PanelKind::Content:
					return ShowContentPanel;
				case Ludus::Editor::Panels::PanelKind::Hierarchy:
					return ShowHierarchyPanel;
				case Ludus::Editor::Panels::PanelKind::Inspector:
					return ShowInspectorPanel;
				case Ludus::Editor::Panels::PanelKind::ImGuiDemo:
					return ShowImGuiDemoPanel;
				case Ludus::Editor::Panels::PanelKind::Dock:
				case Ludus::Editor::Panels::PanelKind::Viewport:
					return true;
			}

			return false;
		}

		void SetVisible(const Ludus::Editor::Panels::PanelKind panelKind, const bool isVisible)
		{
			switch (panelKind)
			{
				case Ludus::Editor::Panels::PanelKind::Console:
					ShowConsolePanel = isVisible;
					return;
				case Ludus::Editor::Panels::PanelKind::Content:
					ShowContentPanel = isVisible;
					return;
				case Ludus::Editor::Panels::PanelKind::Hierarchy:
					ShowHierarchyPanel = isVisible;
					return;
				case Ludus::Editor::Panels::PanelKind::Inspector:
					ShowInspectorPanel = isVisible;
					return;
				case Ludus::Editor::Panels::PanelKind::ImGuiDemo:
					ShowImGuiDemoPanel = isVisible;
					return;
				case Ludus::Editor::Panels::PanelKind::Dock:
				case Ludus::Editor::Panels::PanelKind::Viewport:
					return;
			}
		}
	};
}
