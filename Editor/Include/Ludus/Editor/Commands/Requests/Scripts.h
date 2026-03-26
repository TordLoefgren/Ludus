#pragma once

#include <Ludus/Editor/Commands/RequestCommand.h>

namespace Ludus::Editor::Commands
{
	struct ProjectSessionCommandContext;
}

namespace Ludus::Editor::Commands::Requests::Scripts
{
	void CreateScript(const RequestCommand::CreateScript& command, ProjectSessionCommandContext& context);
}
