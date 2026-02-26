#pragma once

#include <Ludus/Editor/Commands/CommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>

namespace Ludus::Editor::Commands::Requests::Panels
{
	void AddViewport(const RequestCommand::AddViewport& command, CommandContext& context);
	void SetExecutionMode(const RequestCommand::SetExecutionMode& command, CommandContext& context);
}
