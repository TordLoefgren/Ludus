#include "pch.h"

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/Requests/EditorState.h>
#include <Ludus/Editor/Commands/StartupCommandContext.h>
#include <Ludus/UI/Context/ThemeContext.h>

namespace Ludus::Editor::Commands::Requests::EditorState
{
	void SetExecutionFlag(const RequestCommand::SetExecutionFlag& command, ProjectSessionCommandContext& context)
	{
		context.Shell.State.Execution.SetFlag(context.HostContext.GetExecutionFlags(), command.Flag);
	}

	void UnsetExecutionFlag(const RequestCommand::UnsetExecutionFlag& command, ProjectSessionCommandContext& context)
	{
		context.Shell.State.Execution.UnsetFlag(context.HostContext.GetExecutionFlags(), command.Flag);
	}

	void SetExecutionMode(const RequestCommand::SetExecutionMode& command, ProjectSessionCommandContext& context)
	{
		switch (command.Mode)
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

		context.Shell.State.Execution.ExecutionMode = command.Mode;
		context.Shell.State.Execution.Apply(context.HostContext.GetExecutionFlags(), command.Mode);
	}

	void SetTheme(const RequestCommand::SetTheme& command, ProjectSessionCommandContext& context)
	{
		Ludus::UI::Context::ThemeContext::SetActiveTheme(command.ThemeId);
		context.Shell.State.Theme.ActiveThemeId = command.ThemeId;
	}

	void SetExecutionFlag(const RequestCommand::SetExecutionFlag& command, StartupCommandContext& context)
	{
		context.Shell.State.Execution.SetFlag(context.HostContext.GetExecutionFlags(), command.Flag);
	}

	void UnsetExecutionFlag(const RequestCommand::UnsetExecutionFlag& command, StartupCommandContext& context)
	{
		context.Shell.State.Execution.UnsetFlag(context.HostContext.GetExecutionFlags(), command.Flag);
	}
}
