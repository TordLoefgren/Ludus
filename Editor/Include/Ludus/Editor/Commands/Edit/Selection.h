#pragma once

#include <Ludus/Editor/Commands/EditCommand.h>

namespace Ludus::Editor::Commands
{
	struct CommandContext;
}

namespace Ludus::Editor::Commands::Edit::Selection
{
	void SelectEntity(const EditCommand::SelectEntity& command, CommandContext& context);
	void DeselectEntity(const EditCommand::DeselectEntity& command, CommandContext& context);
	void ClearSelection(const EditCommand::ClearSelection& command, CommandContext& context);
}
