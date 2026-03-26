#pragma once

#include <Ludus/Editor/Commands/RequestCommand.h>

namespace Ludus::Editor::Commands
{
	struct ProjectSessionCommandContext;
}

namespace Ludus::Editor::Commands::Requests::Builds
{
	void RunTargetBuildCommand(const RequestCommand::RunTargetBuildCommand& command, ProjectSessionCommandContext& context);
	void BuildRuntime(const RequestCommand::BuildRuntime& command, ProjectSessionCommandContext& context);
	void CleanRuntime(ProjectSessionCommandContext& context);
}
