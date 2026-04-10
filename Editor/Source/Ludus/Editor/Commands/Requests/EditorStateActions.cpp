#include "pch.h"

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/Requests/EditorStateActions.h>
#include <Ludus/Editor/Commands/StartupCommandContext.h>
#include <Ludus/UI/Context/ThemeContext.h>

namespace Ludus::Editor::Commands::Requests::EditorState
{
	void CloseApplicationAction(ProjectSessionCommandContext& context)
	{
		context.HostContext.SetWindowShouldClose();
	}

	void SetExecutionModeAction(Ludus::Editor::Core::ExecutionMode mode, ProjectSessionCommandContext& context)
	{
		switch (mode)
		{
			case Ludus::Editor::Core::ExecutionMode::Start:
				context.ProjectSession.StartSimulation(context.HostContext);
				break;
			case Ludus::Editor::Core::ExecutionMode::Stop:
				context.ProjectSession.StopSimulation(context.HostContext);
				break;
			case Ludus::Editor::Core::ExecutionMode::Pause:
				context.ProjectSession.PauseSimulation(context.HostContext);
				break;
		}

		context.Shell.State.Execution.ExecutionMode = mode;
		context.Shell.State.Execution.Apply(context.HostContext.GetExecutionFlags(), mode);
	}

	void SetExecutionFlagAction(Ludus::Editor::Core::EditorExecutionFlags flag, ProjectSessionCommandContext& context)
	{
		context.Shell.State.Execution.SetFlag(context.HostContext.GetExecutionFlags(), flag);
	}

	void SetThemeAction(Ludus::UI::Theme::ThemeId themeId, ProjectSessionCommandContext& context)
	{
		Ludus::UI::Context::ThemeContext::SetActiveTheme(themeId);
		context.Shell.State.Theme.ActiveThemeId = themeId;
	}

	void UnsetExecutionFlagAction(Ludus::Editor::Core::EditorExecutionFlags flag, ProjectSessionCommandContext& context)
	{
		context.Shell.State.Execution.UnsetFlag(context.HostContext.GetExecutionFlags(), flag);
	}

	void CloseApplicationAction(StartupCommandContext& context)
	{
		context.HostContext.SetWindowShouldClose();
	}

	void SetExecutionFlagAction(Ludus::Editor::Core::EditorExecutionFlags flag, StartupCommandContext& context)
	{
		context.Shell.State.Execution.SetFlag(context.HostContext.GetExecutionFlags(), flag);
	}

	void UnsetExecutionFlagAction(Ludus::Editor::Core::EditorExecutionFlags flag, StartupCommandContext& context)
	{
		context.Shell.State.Execution.UnsetFlag(context.HostContext.GetExecutionFlags(), flag);
	}
}
