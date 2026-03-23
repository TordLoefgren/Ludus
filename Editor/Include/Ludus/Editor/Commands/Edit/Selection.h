#pragma once

#include <Ludus/Editor/Commands/EditCommand.h>

namespace Ludus::Editor::Commands
{
	struct ProjectSessionCommandContext;
}

namespace Ludus::Editor::Commands::Edit::Selection
{
	void SelectEntity(const EditCommand::SelectEntity& command, ProjectSessionCommandContext& context);
	void DeselectEntity(const EditCommand::DeselectEntity& command, ProjectSessionCommandContext& context);
	void ClearSelection(const EditCommand::ClearSelection& command, ProjectSessionCommandContext& context);
}
