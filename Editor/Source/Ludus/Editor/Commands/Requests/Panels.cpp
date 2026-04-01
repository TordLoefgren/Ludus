#include "pch.h"

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/Requests/Panels.h>
#include <Ludus/Editor/Panels/PanelRegistry.h>
#include <Ludus/Editor/Panels/ViewportPanel.h>
#include <Ludus/UI/Context/ThemeContext.h>

namespace Ludus::Editor::Commands::Requests::Panels
{
	void AddViewport(ProjectSessionCommandContext& context)
	{
		context.PanelRegistry.Register(std::make_unique<Ludus::Editor::Panels::ViewportPanel>());
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

	void SetPanelVisibility(const RequestCommand::SetPanelVisibility& command, ProjectSessionCommandContext& context)
	{
		context.Shell.State.Panels.SetVisible(command.PanelKind, command.IsVisible);
	}

	void SetTheme(const RequestCommand::SetTheme& command, ProjectSessionCommandContext& context)
	{
		Ludus::UI::Context::ThemeContext::SetActiveTheme(command.ThemeId);
		context.Shell.State.Theme.ActiveThemeId = command.ThemeId;
	}
}
