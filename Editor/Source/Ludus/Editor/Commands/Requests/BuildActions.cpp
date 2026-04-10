#include "pch.h"

#include <Ludus/Editor/Build/BuildCommand.h>
#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildTarget.h>
#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/Requests/BuildActions.h>
#include <Ludus/Editor/Panels/PanelHelpers.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Editor::Commands::Requests::Builds
{
	void BuildRuntimeAction(
		Ludus::Editor::Build::BuildConfiguration buildConfiguration,
		ProjectSessionCommandContext& context
	)
	{
		LUDUS_ASSERT(
			!context.ProjectSession.RuntimeState.IsSimulationActive(),
			"Build command cannot be executed while the simulation session is active."
		);

		const auto projectRoot = context.ProjectSession.Persistence.GetProjectRoot();
		auto& build = context.Shell.Build;
		build.BuildRuntime(projectRoot, buildConfiguration);

		Ludus::Editor::Panels::RefreshContentPanel(projectRoot, context.PanelRegistry);
	}

	void CleanRuntimeAction(ProjectSessionCommandContext& context)
	{
		LUDUS_ASSERT(
			!context.ProjectSession.RuntimeState.IsSimulationActive(),
			"Build command cannot be executed while the simulation session is active."
		);

		const auto projectRoot = context.ProjectSession.Persistence.GetProjectRoot();
		auto& build = context.Shell.Build;
		build.CleanRuntime(projectRoot);

		Ludus::Editor::Panels::RefreshContentPanel(projectRoot, context.PanelRegistry);
	}

	void RunTargetBuildCommandAction(
		Ludus::Editor::Build::BuildTarget buildTarget,
		Ludus::Editor::Build::BuildCommand buildCommand,
		Ludus::Editor::Build::BuildConfiguration buildConfiguration,
		ProjectSessionCommandContext& context
	)
	{
		LUDUS_ASSERT(
			!context.ProjectSession.RuntimeState.IsSimulationActive(),
			"Build command cannot be executed while the simulation session is active."
		);

		const auto projectRoot = context.ProjectSession.Persistence.GetProjectRoot();
		auto& build = context.Shell.Build;
		build.RunTargetBuildCommand(projectRoot, buildTarget, buildCommand, buildConfiguration);

		Ludus::Editor::Panels::RefreshContentPanel(projectRoot, context.PanelRegistry);
	}
}
