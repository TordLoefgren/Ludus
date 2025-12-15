#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <Ludus/Editor/Panels/ConsolePanel.h>
#include <Ludus/Editor/Panels/DockPanel.h>
#include <Ludus/Editor/Panels/HierarchyPanel.h>
#include <Ludus/Editor/Panels/ImGuiDemoPanel.h>
#include <Ludus/Editor/Panels/InspectorPanel.h>
#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/Editor/Panels/ViewportPanel.h>

namespace Ludus::Editor::Core
{
	using PanelFactory = std::function<std::unique_ptr<Ludus::Editor::Panels::IPanel>()>;

	struct EditorConfiguration
	{
		std::vector<PanelFactory> PanelFactories;

		static EditorConfiguration Default()
		{
			EditorConfiguration options;
			options.PanelFactories.push_back([] { return std::make_unique<Ludus::Editor::Panels::DockPanel>(); });
			options.PanelFactories.push_back([] { return std::make_unique<Ludus::Editor::Panels::ImGuiDemoPanel>(); });
			options.PanelFactories.push_back([] { return std::make_unique<Ludus::Editor::Panels::HierarchyPanel>(); });
			options.PanelFactories.push_back([] { return std::make_unique<Ludus::Editor::Panels::InspectorPanel>(); });
			options.PanelFactories.push_back([] { return std::make_unique<Ludus::Editor::Panels::ConsolePanel>(); });
			options.PanelFactories.push_back([] { return std::make_unique<Ludus::Editor::Panels::ViewportPanel>(); });

			return options;
		}
	};
}
