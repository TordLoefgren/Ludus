#pragma once

#include <filesystem>
#include <string>

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/Requests/ProjectTransitionContext.h>
#include <Ludus/Editor/Commands/StartupCommandContext.h>

namespace Ludus::Editor::Commands::Requests::Projects
{
	void CreateProjectAction(const std::string& name, ProjectTransitionContext context);
	void CreateProjectAsAction(const std::filesystem::path& path, const std::string& name, ProjectTransitionContext context);
	void OpenProjectAction(const std::filesystem::path& path, ProjectTransitionContext context);
	void CloseProjectAction(ProjectSessionCommandContext& context);
	void SaveProjectAction(ProjectSessionCommandContext& context);
}
