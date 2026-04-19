#pragma once

#include <Ludus/Editor/Commands/CommandManager.h>
#include <Ludus/Editor/Core/ActivePanelState.h>
#include <Ludus/Editor/Core/ActiveThemeState.h>
#include <Ludus/Editor/Core/EditorMode.h>
#include <Ludus/Editor/Core/ExecutionManager.h>
#include <Ludus/Editor/Core/PendingProjectTransition.h>
#include <Ludus/Editor/Dialogs/DialogManager.h>

namespace Ludus::Editor::Core
{
	struct EditorState
	{
		Ludus::Editor::Core::ActivePanelState Panels;
		Ludus::Editor::Core::ActiveThemeState Theme;
		Ludus::Editor::Commands::CommandManager Commands;
		Ludus::Editor::Dialogs::DialogManager Dialogs;
		Ludus::Editor::Core::ExecutionManager Execution;
		Ludus::Editor::Core::EditorMode Mode = Ludus::Editor::Core::EditorMode::Startup;
		Ludus::Editor::Core::PendingProjectTransition PendingProjectTransition = PendingProjectTransition::NoneState();
	};
}
