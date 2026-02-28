#pragma once

namespace Ludus::Editor::Core
{
	struct EditorContext;
}

namespace Ludus::Editor::Panels
{
	struct PanelRegistry;
}

namespace Ludus::Engine::Core
{
	struct SystemContext;
}

namespace Ludus::Editor::Commands
{
	struct CommandContext
	{
		Ludus::Editor::Core::EditorContext& EditorContext;
		Ludus::Engine::Core::SystemContext& SystemContext;
		Ludus::Editor::Panels::PanelRegistry& Panels;
	};
}
