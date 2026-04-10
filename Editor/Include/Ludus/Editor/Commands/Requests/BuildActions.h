#pragma once

#include <Ludus/Editor/Build/BuildCommand.h>
#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildTarget.h>
#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>

namespace Ludus::Editor::Commands::Requests::Builds
{
	void BuildRuntimeAction(
		Ludus::Editor::Build::BuildConfiguration buildConfiguration,
		ProjectSessionCommandContext& context
	);

	void CleanRuntimeAction(ProjectSessionCommandContext& context);

	void RunTargetBuildCommandAction(
		Ludus::Editor::Build::BuildTarget buildTarget,
		Ludus::Editor::Build::BuildCommand buildCommand,
		Ludus::Editor::Build::BuildConfiguration buildConfiguration,
		ProjectSessionCommandContext& context
	);
}
