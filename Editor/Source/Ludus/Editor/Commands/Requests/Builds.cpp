#include "pch.h"

#include <stdexcept>

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Panels/PanelHelpers.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Editor::Commands::Requests::Builds
{
	void RunTargetBuildCommand(const RequestCommand::RunTargetBuildCommand& command, ProjectSessionCommandContext& context)
	{
		LUDUS_ASSERT(!context.ProjectSession.IsSimulating(), "Build command cannot be executed while the simulation session is active.");

		const auto projectRoot = context.ProjectSession.GetProjectRoot();
		auto& build = context.Shell.Build;
		build.RunTargetBuildCommand(projectRoot, command.BuildTarget, command.BuildCommand, command.BuildConfiguration);

		Ludus::Editor::Panels::RefreshContentPanel(projectRoot, context.PanelRegistry);
	}

	void BuildRuntime(const RequestCommand::BuildRuntime& command, ProjectSessionCommandContext& context)
	{
		LUDUS_ASSERT(!context.ProjectSession.IsSimulating(), "Build command cannot be executed while the simulation session is active.");

		const auto projectRoot = context.ProjectSession.GetProjectRoot();
		auto& build = context.Shell.Build;
		build.BuildRuntime(projectRoot, command.BuildConfiguration);

		Ludus::Editor::Panels::RefreshContentPanel(projectRoot, context.PanelRegistry);
	}

	void CleanRuntime(ProjectSessionCommandContext& context)
	{
		LUDUS_ASSERT(!context.ProjectSession.IsSimulating(), "Build command cannot be executed while the simulation session is active.");

		const auto projectRoot = context.ProjectSession.GetProjectRoot();
		auto& build = context.Shell.Build;
		build.CleanRuntime(projectRoot);

		Ludus::Editor::Panels::RefreshContentPanel(projectRoot, context.PanelRegistry);
	}
}
