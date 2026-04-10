#pragma once

#include <Ludus/Editor/Commands/RequestCommand.h>

namespace Ludus::Editor::Commands
{
	struct ProjectSessionCommandContext;
	struct StartupCommandContext;
}

namespace Ludus::Editor::Commands::Requests::EditorState
{
	void CloseApplication(ProjectSessionCommandContext& context);
	void ResolveUnsavedChanges(const RequestCommand::ResolveUnsavedChanges& command, ProjectSessionCommandContext& context);
	void SetExecutionMode(const RequestCommand::SetExecutionMode& command, ProjectSessionCommandContext& context);
	void SetExecutionFlag(const RequestCommand::SetExecutionFlag& command, ProjectSessionCommandContext& context);
	void SetTheme(const RequestCommand::SetTheme& command, ProjectSessionCommandContext& context);
	void UnsetExecutionFlag(const RequestCommand::UnsetExecutionFlag& command, ProjectSessionCommandContext& context);

	void CloseApplication(StartupCommandContext& context);
	void SetExecutionFlag(const RequestCommand::SetExecutionFlag& command, StartupCommandContext& context);
	void UnsetExecutionFlag(const RequestCommand::UnsetExecutionFlag& command, StartupCommandContext& context);
}
