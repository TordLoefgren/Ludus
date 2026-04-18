#pragma once

#include <Ludus/Editor/Core/EditorPersistenceContext.h>
#include <Ludus/Editor/Core/EditorPreferences.h>
#include <Ludus/Editor/Core/EditorShell.h>
#include <Ludus/Editor/Core/ProjectSession.h>
#include <Ludus/Editor/Panels/PanelRegistry.h>
#include <Ludus/Engine/Runtime/IHostContext.h>

namespace Ludus::Editor::Commands
{
	struct ProjectSessionCommandContext
	{
		Ludus::Editor::Core::EditorShell& Shell;
		Ludus::Engine::Runtime::IHostContext& HostContext;
		Ludus::Editor::Core::ProjectSession& ProjectSession;
		Ludus::Editor::Core::EditorPreferences& Preferences;
		Ludus::Editor::Panels::PanelRegistry& PanelRegistry;
		Ludus::Editor::Core::EditorPersistenceContext Persistence;
	};
}
