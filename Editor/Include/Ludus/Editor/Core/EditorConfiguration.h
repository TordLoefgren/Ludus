#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <vector>

#include <Ludus/Editor/Core/ViewportDisplayMode.h>
#include <Ludus/Editor/Panels/ConsolePanel.h>
#include <Ludus/Editor/Panels/ContentPanel.h>
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

		EditorConfiguration() = default;

		static EditorConfiguration Default()
		{
			EditorConfiguration configuration;

			// Register the dock panel first to ensure a docking surface is available.
			configuration.PanelFactories.push_back([] { return std::make_unique<Ludus::Editor::Panels::DockPanel>(); });

			// Single-instance panels.
			configuration.PanelFactories.push_back([] { return std::make_unique<Ludus::Editor::Panels::ConsolePanel>(); });
			configuration.PanelFactories.push_back([] { return std::make_unique<Ludus::Editor::Panels::ContentPanel>(); });
			configuration.PanelFactories.push_back([] { return std::make_unique<Ludus::Editor::Panels::HierarchyPanel>(); });
			configuration.PanelFactories.push_back([] { return std::make_unique<Ludus::Editor::Panels::ImGuiDemoPanel>(); });
			configuration.PanelFactories.push_back([] { return std::make_unique<Ludus::Editor::Panels::InspectorPanel>(); });

			// Multi-instance panels.
			configuration.PanelFactories.push_back([] { return std::make_unique<Ludus::Editor::Panels::ViewportPanel>("Viewport", Ludus::Editor::Core::ViewportDisplayMode::Simulation); });
			configuration.PanelFactories.push_back([] { return std::make_unique<Ludus::Editor::Panels::ViewportPanel>("Viewport", Ludus::Editor::Core::ViewportDisplayMode::Editor); });

			return configuration;
		}

		EditorConfiguration(const EditorConfiguration&) = delete;
		EditorConfiguration& operator=(const EditorConfiguration&) = delete;
		EditorConfiguration(EditorConfiguration&&) = default;
		EditorConfiguration& operator=(EditorConfiguration&&) = default;
	};
}
