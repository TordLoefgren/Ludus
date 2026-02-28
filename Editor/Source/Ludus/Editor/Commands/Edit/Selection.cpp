#include "pch.h"

#include <Ludus/Editor/Commands/CommandContext.h>
#include <Ludus/Editor/Commands/Edit/Selection.h>
#include <Ludus/Editor/Core/EditorContext.h>

namespace Ludus::Editor::Commands::Edit::Selection
{
	void SelectEntity(const EditCommand::SelectEntity& command, CommandContext& context)
	{
		const auto entity = context.EditorContext.State.Commands.ResolveEntity(command.Entity);
		context.EditorContext.State.Selection.SelectEntity(entity);
	}

	void DeselectEntity(const EditCommand::DeselectEntity& command, CommandContext& context)
	{
		const auto entity = context.EditorContext.State.Commands.ResolveEntity(command.Entity);
		context.EditorContext.State.Selection.DeselectEntity(entity);
	}

	void ClearSelection(const EditCommand::ClearSelection& command, CommandContext& context)
	{
		context.EditorContext.State.Selection.ClearSelection();
	}
}
