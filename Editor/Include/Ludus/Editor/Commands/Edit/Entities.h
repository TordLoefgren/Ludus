#pragma once

#include <Ludus/Editor/Commands/EditCommand.h>

namespace Ludus::Editor::Commands
{
	struct CommandContext;
}

namespace Ludus::Editor::Commands::Edit::Entities
{
	void AddEntity(const EditCommand::AddEntity& command, CommandContext& context);
	void RemoveEntity(const EditCommand::RemoveEntity& command, CommandContext& context);
}
