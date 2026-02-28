#pragma once

#include <Ludus/Editor/Commands/CommandManager.h>
#include <Ludus/Editor/Core/ExecutionManager.h>
#include <Ludus/Editor/Core/SelectionManager.h>
#include <Ludus/Editor/Dialogs/DialogManager.h>

namespace Ludus::Editor::Core
{
	struct EditorState
	{
		Ludus::Editor::Dialogs::DialogManager Dialogs;
		Ludus::Editor::Core::ExecutionManager Execution;
		Ludus::Editor::Core::SelectionManager Selection;
		Ludus::Editor::Commands::CommandManager Commands;
	};
}
