#pragma once

#include <Ludus/Editor/Commands/UICommand.h>

namespace Ludus::Editor::Commands
{
	struct CommandContext;
}

namespace Ludus::Editor::Commands::UI::Dialogs
{
	void OpenAddScriptDialog(const UICommand::OpenAddScriptDialog& command, CommandContext& context);
	void OpenCreateProjectDialog(const UICommand::OpenCreateProjectDialog& command, CommandContext& context);
}
