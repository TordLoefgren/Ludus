#pragma once

#include <Ludus/Editor/Commands/RequestCommand.h>

namespace Ludus::Editor::Commands
{
	struct ProjectSessionCommandContext;
	struct StartupCommandContext;
}

namespace Ludus::Editor::Commands::Requests::Projects
{
	void CreateProject(const RequestCommand::CreateProject& command, StartupCommandContext& context);
	void CreateProjectAs(const RequestCommand::CreateProjectAs& command, StartupCommandContext& context);
	void OpenProject(const RequestCommand::OpenProject& command, StartupCommandContext& context);

	void CreateProject(const RequestCommand::CreateProject& command, ProjectSessionCommandContext& context);
	void CreateProjectAs(const RequestCommand::CreateProjectAs& command, ProjectSessionCommandContext& context);
	void OpenProject(const RequestCommand::OpenProject& command, ProjectSessionCommandContext& context);
	void SaveProject(ProjectSessionCommandContext& context);
	void CloseProject(ProjectSessionCommandContext& context);
}
