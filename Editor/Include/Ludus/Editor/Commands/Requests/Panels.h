#pragma once

#include <Ludus/Editor/Commands/RequestCommand.h>

namespace Ludus::Editor::Commands
{
	struct ProjectSessionCommandContext;
}

namespace Ludus::Editor::Commands::Requests::Panels
{
	void AddViewport(ProjectSessionCommandContext& context);
	void SetExecutionMode(const RequestCommand::SetExecutionMode& command, ProjectSessionCommandContext& context);
}
