#include "pch.h"

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/Requests/Panels.h>
#include <Ludus/Editor/Panels/PanelRegistry.h>
#include <Ludus/Editor/Panels/ViewportPanel.h>

namespace Ludus::Editor::Commands::Requests::Panels
{
	void AddViewport(ProjectSessionCommandContext& context)
	{
		context.PanelRegistry.Register(std::make_unique<Ludus::Editor::Panels::ViewportPanel>());
	}

	void SetPanelVisibility(const RequestCommand::SetPanelVisibility& command, ProjectSessionCommandContext& context)
	{
		context.Shell.State.Panels.SetVisible(command.PanelKind, command.IsVisible);
	}
}
