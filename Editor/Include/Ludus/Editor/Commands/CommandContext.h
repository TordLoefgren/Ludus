#pragma once

#include <Ludus/Editor/Core/EditorContext.h>
#include <Ludus/Editor/Panels/PanelRegistry.h>
#include <Ludus/Engine/Core/SystemContext.h>

namespace Ludus::Editor::Commands
{
	struct CommandContext
	{
		Ludus::Editor::Core::EditorContext& EditorContext;
		Ludus::Engine::Core::SystemContext& SystemContext;
		Ludus::Editor::Panels::PanelRegistry& Panels;
	};
}
