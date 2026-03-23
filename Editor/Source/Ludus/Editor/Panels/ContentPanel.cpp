#include "pch.h"

#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Panels/ContentPanel.h>
#include <Ludus/UI/Context/ScrollContext.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Text.h>

namespace Ludus::Editor::Panels
{
	bool ContentPanel::UpdateImpl(Ludus::Editor::Core::ProjectSessionContext& context)
	{
		const auto flags = Ludus::Editor::Core::Constants::PanelFlags | Ludus::UI::Flags::Window::HorizontalScrollbar;
		auto windowTitle = CreateWindowTitle("Content");

		if (Ludus::UI::Scope::WindowScope window(windowTitle.c_str(), &m_Open, flags); window)
		{
			m_AssetBrowser.Update();

			if (auto openedFilePath = m_AssetBrowser.ConsumeOpenedFilePath(); openedFilePath.has_value())
			{
				if (openedFilePath->filename().generic_string().ends_with(".ludus.scene"))
				{
					context.Shell.State.Commands.AddRequestCommand(
						Ludus::Editor::Commands::RequestCommand::OpenScene { *openedFilePath }
					);
				}
			}
		}

		return true;
	}
}
