#pragma once

#include <Ludus/Editor/Commands/CommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Core/ExecutionMode.h>
#include <Ludus/Editor/Panels/ViewportPanel.h>

namespace Ludus::Editor::Commands::Requests::Panels
{
	void AddViewport(const RequestCommand::AddViewport& command, CommandContext& context)
	{
		context.Panels.Register(std::make_unique<Ludus::Editor::Panels::ViewportPanel>());
	}

	void SetExecutionMode(const RequestCommand::SetExecutionMode& command, CommandContext& context)
	{
		context.EditorContext.State.ExecutionMode = command.Mode;
		context.EditorContext.State.ExecutionManager.Apply(context.SystemContext, command.Mode);
	}
}
