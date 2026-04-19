#pragma once

#include <Ludus/Editor/Core/EditorShell.h>
#include <Ludus/Editor/Core/ProjectSession.h>
#include <Ludus/Editor/Panels/PanelRegistry.h>
#include <Ludus/Engine/Runtime/IHostContext.h>

namespace Ludus::Editor::Core
{
	struct EditorShortcutContext
	{
		Ludus::Editor::Core::EditorShell& Shell;
		Ludus::Engine::Runtime::IHostContext& HostContext;
		Ludus::Editor::Panels::PanelRegistry& PanelRegistry;
		Ludus::Editor::Core::ProjectSession* ProjectSession = nullptr;
	};
}
