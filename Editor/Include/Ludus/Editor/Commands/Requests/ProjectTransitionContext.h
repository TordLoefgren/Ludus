#pragma once

#include <Ludus/Editor/Core/EditorPersistenceContext.h>
#include <Ludus/Editor/Core/EditorPreferences.h>
#include <Ludus/Editor/Core/EditorShell.h>
#include <Ludus/Editor/Panels/PanelRegistry.h>

namespace Ludus::Editor::Commands::Requests::Projects
{
	struct ProjectTransitionContext
	{
		Ludus::Editor::Core::EditorShell& Shell;
		Ludus::Editor::Core::EditorPreferences& Preferences;
		Ludus::Editor::Panels::PanelRegistry& PanelRegistry;
		Ludus::Editor::Core::EditorPersistenceContext& Persistence;
	};
}
