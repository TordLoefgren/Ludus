#pragma once

#include <type_traits>
#include <utility>

#include <Ludus/Editor/Commands/EditCommand.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/UICommand.h>
#include <Ludus/Editor/Core/EditorContext.h>

namespace Ludus::Editor::Commands
{
	inline void EnqueueEdit(Ludus::Editor::Core::EditorContext& editorContext, EditCommand command)
	{
		editorContext.State.Commands.AddEditCommand(std::move(command));
	}

	template<typename TCommand>
		requires (!std::is_same_v<std::remove_cvref_t<TCommand>, EditCommand>)
	inline void EnqueueEdit(Ludus::Editor::Core::EditorContext& editorContext, TCommand&& command)
	{
		EnqueueEdit(editorContext, EditCommand { std::forward<TCommand>(command) });
	}

	inline void EnqueueRequest(Ludus::Editor::Core::EditorContext& editorContext, RequestCommand command)
	{
		editorContext.State.Commands.AddRequestCommand(std::move(command));
	}

	template<typename TCommand>
		requires (!std::is_same_v<std::remove_cvref_t<TCommand>, RequestCommand>)
	inline void EnqueueRequest(Ludus::Editor::Core::EditorContext& editorContext, TCommand&& command)
	{
		EnqueueRequest(editorContext, RequestCommand { std::forward<TCommand>(command) });
	}

	inline void EnqueueUI(Ludus::Editor::Core::EditorContext& editorContext, UICommand command)
	{
		editorContext.State.Commands.AddUICommand(std::move(command));
	}

	template<typename TCommand>
		requires (!std::is_same_v<std::remove_cvref_t<TCommand>, UICommand>)
	inline void EnqueueUI(Ludus::Editor::Core::EditorContext& editorContext, TCommand&& command)
	{
		EnqueueUI(editorContext, UICommand { std::forward<TCommand>(command) });
	}
}
