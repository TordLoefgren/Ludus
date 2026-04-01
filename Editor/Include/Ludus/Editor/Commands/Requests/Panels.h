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
	void SetPanelVisibility(const RequestCommand::SetPanelVisibility& command, ProjectSessionCommandContext& context);
	void SetTheme(const RequestCommand::SetTheme& command, ProjectSessionCommandContext& context);
}
