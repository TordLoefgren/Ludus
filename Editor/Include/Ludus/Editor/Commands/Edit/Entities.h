#pragma once

#include <Ludus/Editor/Commands/EditCommand.h>

namespace Ludus::Editor::Commands
{
	struct ProjectSessionCommandContext;
}

namespace Ludus::Editor::Commands::Edit::Entities
{
	void AddEntity(const EditCommand::AddEntity& command, ProjectSessionCommandContext& context);
	void RemoveEntity(const EditCommand::RemoveEntity& command, ProjectSessionCommandContext& context);
}
