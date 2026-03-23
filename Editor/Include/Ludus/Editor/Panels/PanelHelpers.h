#pragma once

#include <filesystem>

#include <Ludus/Editor/Panels/ConsolePanel.h>
#include <Ludus/Editor/Panels/ContentPanel.h>
#include <Ludus/Editor/Panels/PanelRegistry.h>

namespace Ludus::Editor::Panels
{
	inline void RefreshContentPanel(const std::filesystem::path& projectRoot, PanelRegistry& panelRegistry)
	{
		auto* contentPanel = panelRegistry.TryGet<ContentPanel>();
		if (!contentPanel)
		{
			return;
		}

		contentPanel->Initialize(projectRoot);
	}

	inline void ClearConsolePanel(PanelRegistry& panelRegistry)
	{
		auto* consolePanel = panelRegistry.TryGet<ConsolePanel>();
		if (!consolePanel)
		{
			return;
		}

		consolePanel->Clear();
	}
}
