#include "pch.h"

#include <Ludus/Editor/Commands/Edit/Selection.h>
#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>

namespace Ludus::Editor::Commands::Edit::Selection
{
	void SelectEntity(const EditCommand::SelectEntity& command, ProjectSessionCommandContext& context)
	{
		const auto entity = context.Shell.State.Commands.ResolveEntity(command.EntityReference);
		context.ProjectSession.EditorState.Selection.SelectEntity(entity);
	}

	void DeselectEntity(const EditCommand::DeselectEntity& command, ProjectSessionCommandContext& context)
	{
		const auto entity = context.Shell.State.Commands.ResolveEntity(command.EntityReference);
		context.ProjectSession.EditorState.Selection.DeselectEntity(entity);
	}

	void ClearSelection(const EditCommand::ClearSelection& command, ProjectSessionCommandContext& context)
	{
		context.ProjectSession.EditorState.Selection.ClearSelection();
	}
}
