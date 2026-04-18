#pragma once

#include <Ludus/Editor/Core/EditorExecutionFlags.h>
#include <Ludus/Editor/Core/ExecutionMode.h>
#include <Ludus/UI/Theme/ThemeId.h>

namespace Ludus::Editor::Commands
{
	struct ProjectSessionCommandContext;
	struct StartupCommandContext;
}

namespace Ludus::Editor::Commands::Requests::EditorState
{
	void CloseApplicationAction(ProjectSessionCommandContext& context);
	void SetExecutionModeAction(Ludus::Editor::Core::ExecutionMode mode, ProjectSessionCommandContext& context);
	void SetExecutionFlagAction(Ludus::Editor::Core::EditorExecutionFlags flag, ProjectSessionCommandContext& context);
	void SetThemeAction(Ludus::UI::Theme::ThemeId themeId, ProjectSessionCommandContext& context);
	void UnsetExecutionFlagAction(Ludus::Editor::Core::EditorExecutionFlags flag, ProjectSessionCommandContext& context);

	void CloseApplicationAction(StartupCommandContext& context);
	void SetExecutionFlagAction(Ludus::Editor::Core::EditorExecutionFlags flag, StartupCommandContext& context);
	void SetThemeAction(Ludus::UI::Theme::ThemeId themeId, StartupCommandContext& context);
	void UnsetExecutionFlagAction(Ludus::Editor::Core::EditorExecutionFlags flag, StartupCommandContext& context);
}
