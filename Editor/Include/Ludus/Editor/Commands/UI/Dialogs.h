#pragma once

#include <Ludus/Editor/Commands/UICommand.h>

namespace Ludus::Editor::Commands
{
	struct StartupCommandContext;
	struct ProjectSessionCommandContext;
}

namespace Ludus::Editor::Commands::UI::Dialogs
{
	void OpenAddScriptDialog(const UICommand::OpenAddScriptDialog& command, ProjectSessionCommandContext& context);
	void OpenCreateProjectDialog(ProjectSessionCommandContext& context);
	void OpenRenameSceneDialog(const UICommand::OpenRenameSceneDialog& command, ProjectSessionCommandContext& context);
	void OpenUnsavedChangesDialog(const UICommand::OpenUnsavedChangesDialog& command, ProjectSessionCommandContext& context);

	void OpenCreateProjectDialog(StartupCommandContext& context);
}
