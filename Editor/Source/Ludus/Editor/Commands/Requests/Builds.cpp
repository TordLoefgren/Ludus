#include "pch.h"

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/Requests/BuildActions.h>
#include <Ludus/Editor/Commands/Requests/DeferredAction.h>

namespace Ludus::Editor::Commands::Requests::Builds
{
	void BuildRuntime(const RequestCommand::BuildRuntime& command, ProjectSessionCommandContext& context)
	{
		auto action = DeferredAction::BuildRuntime(command.BuildConfiguration);
		if (TryOpenUnsavedChangesDialog(action, context))
		{
			return;
		}

		ExecuteDeferredAction(action, context);
	}

	void CleanRuntime(ProjectSessionCommandContext& context)
	{
		Ludus::Editor::Commands::Requests::Builds::CleanRuntimeAction(context);
	}

	void RunTargetBuildCommand(const RequestCommand::RunTargetBuildCommand& command, ProjectSessionCommandContext& context)
	{
		Ludus::Editor::Commands::Requests::Builds::RunTargetBuildCommandAction(
			command.BuildTarget,
			command.BuildCommand,
			command.BuildConfiguration,
			context
		);
	}
}
