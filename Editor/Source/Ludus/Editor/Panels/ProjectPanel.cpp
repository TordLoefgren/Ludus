#include "pch.h"

#include <format>

#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Panels/ProjectPanel.h>
#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/UI/Context/ScrollContext.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Text.h>

namespace Ludus::Editor::Panels
{
	bool ProjectPanel::UpdateImpl(Ludus::Editor::Panels::PanelContext& context)
	{
		const auto flags = Ludus::Editor::Core::Constants::PanelFlags | Ludus::UI::Flags::Window::HorizontalScrollbar;
		auto windowTitle = CreateWindowTitle("Project");

		if (Ludus::UI::Scope::WindowScope window(windowTitle.c_str(), &m_Open, flags); window)
		{
			if (context.SystemContext.ProjectContext.has_value())
			{
				m_AssetBrowser.Update();

				if (auto openedFilePath = m_AssetBrowser.ConsumeOpenedFilePath(); openedFilePath.has_value())
				{
					if (openedFilePath->extension() == ".lscene")
					{
						context.EditorContext.State.Commands.AddRequestCommand(
							Ludus::Editor::Commands::RequestCommand::OpenScene { *openedFilePath }
						);
					}
				}
			}
		}

		return true;
	}

	void ProjectPanel::Refresh(std::optional< Ludus::Engine::Persistence::ProjectContext> projectContext)
	{
		if (!projectContext.has_value())
		{
			Clear();
			return;
		}

		const auto assetsDirectory = Ludus::Engine::Persistence::Paths::AssetsDirectory(projectContext.value().ProjectRootDirectory);
		Initialize(assetsDirectory);
	}
}
