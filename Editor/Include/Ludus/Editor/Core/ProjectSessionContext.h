#pragma once

#include <Ludus/Editor/Core/EditorShell.h>
#include <Ludus/Editor/Core/ProjectSession.h>
#include <Ludus/Engine/Runtime/IHostContext.h>

namespace Ludus::Editor::Panels
{
	struct PanelRegistry;
}

namespace Ludus::Editor::Core
{
	struct ProjectSessionContext
	{
		EditorShell& Shell;
		ProjectSession& ProjectSession;
		Ludus::Engine::Runtime::IHostContext& HostContext;
		Ludus::Editor::Panels::PanelRegistry& PanelRegistry;
	};
}
