#pragma once

#include <vector>

#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Core/EditorExecutionManager.h>
#include <Ludus/Editor/Core/EditorSelection.h>

namespace Ludus::Editor::Core
{
	struct EditorState
	{
		Ludus::Editor::Core::ExecutionMode ExecutionMode = ExecutionMode::Stop;
		Ludus::Editor::Core::EditorExecutionManager ExecutionManager;
		std::vector<Ludus::Editor::Commands::RequestCommand> RequestCommands;
		Ludus::Editor::Core::EditorSelection Selection;

		void AddRequestCommand(Ludus::Editor::Commands::RequestCommand command)
		{
			RequestCommands.emplace_back(std::move(command));
		}
	};
}
