#include "pch.h"

#include <memory>

#include <Ludus/Editor/Commands/Requests/Panels.h>
#include <Ludus/Editor/Core/EditorContext.h>
#include <Ludus/Editor/Panels/PanelRegistry.h>
#include <Ludus/Editor/Panels/ViewportPanel.h>
#include <Ludus/Engine/Core/SystemContext.h>

namespace Ludus::Editor::Commands::Requests::Panels
{
	void AddViewport(const RequestCommand::AddViewport& command, CommandContext& context)
	{
		context.Panels.Register(std::make_unique<Ludus::Editor::Panels::ViewportPanel>());
	}

	void SetExecutionMode(const RequestCommand::SetExecutionMode& command, CommandContext& context)
	{
		context.EditorContext.State.Execution.ExecutionMode = command.Mode;
		context.EditorContext.State.Execution.Apply(context.SystemContext, command.Mode);
	}
}
