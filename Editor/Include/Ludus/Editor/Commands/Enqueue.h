#pragma once

#include <type_traits>
#include <utility>

#include <Ludus/Editor/Commands/CommandManager.h>
#include <Ludus/Editor/Commands/EditCommand.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/UICommand.h>

namespace Ludus::Editor::Commands
{
	inline void EnqueueEdit(CommandManager& commands, EditCommand command)
	{
		commands.AddEditCommand(std::move(command));
	}

	template<typename TCommand>
		requires (!std::is_same_v<std::remove_cvref_t<TCommand>, EditCommand>)
	inline void EnqueueEdit(CommandManager& commands, TCommand&& command)
	{
		EnqueueEdit(commands, EditCommand { std::forward<TCommand>(command) });
	}

	inline void EnqueueRequest(CommandManager& commands, RequestCommand command)
	{
		commands.AddRequestCommand(std::move(command));
	}

	template<typename TCommand>
		requires (!std::is_same_v<std::remove_cvref_t<TCommand>, RequestCommand>)
	inline void EnqueueRequest(CommandManager& commands, TCommand&& command)
	{
		EnqueueRequest(commands, RequestCommand { std::forward<TCommand>(command) });
	}

	inline void EnqueueUI(CommandManager& commands, UICommand command)
	{
		commands.AddUICommand(std::move(command));
	}

	template<typename TCommand>
		requires (!std::is_same_v<std::remove_cvref_t<TCommand>, UICommand>)
	inline void EnqueueUI(CommandManager& commands, TCommand&& command)
	{
		EnqueueUI(commands, UICommand { std::forward<TCommand>(command) });
	}
}
