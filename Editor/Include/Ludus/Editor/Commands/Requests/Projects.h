#pragma once

#include <Ludus/Editor/Commands/CommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>

namespace Ludus::Editor::Commands::Requests::Projects
{
	void CreateProject(const RequestCommand::CreateProject& command, CommandContext& context);
	void OpenProject(const RequestCommand::OpenProject& command, CommandContext& context);
	void CloseProject(const RequestCommand::CloseProject& command, CommandContext& context);
}
