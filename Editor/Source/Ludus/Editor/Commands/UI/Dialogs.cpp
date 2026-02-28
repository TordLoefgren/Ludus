#include "pch.h"

#include <Ludus/Editor/Commands/CommandContext.h>
#include <Ludus/Editor/Commands/UI/Dialogs.h>
#include <Ludus/Editor/Core/EditorContext.h>
#include <Ludus/Editor/Dialogs/CreateProjectDialog.h>
#include <Ludus/Editor/Dialogs/DialogManager.h>

namespace Ludus::Editor::Commands::UI::Dialogs
{
	void OpenCreateProjectDialog(const UICommand::OpenCreateProjectDialog& command, CommandContext& context)
	{
		(void)command;
		context.EditorContext.State.Dialogs.Open<Ludus::Editor::Dialogs::CreateProjectDialog>();
	}
}
