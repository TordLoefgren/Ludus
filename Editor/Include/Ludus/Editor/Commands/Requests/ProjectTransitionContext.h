#pragma once

#include <Ludus/Editor/Core/EditorShell.h>
#include <Ludus/Editor/Panels/PanelRegistry.h>
#include <Ludus/Editor/Persistence/IProjectManifestPersistence.h>
#include <Ludus/Engine/Persistence/IRuntimeLaunchSettingsPersistence.h>
#include <Ludus/Engine/Persistence/IRuntimeManifestPersistence.h>
#include <Ludus/Engine/Persistence/IScenePersistence.h>

namespace Ludus::Editor::Commands
{
	struct ProjectSessionCommandContext;
	struct StartupCommandContext;
}

namespace Ludus::Editor::Commands::Requests::Projects
{
	struct ProjectTransitionContext
	{
		Ludus::Editor::Core::EditorShell& Shell;
		Ludus::Engine::Persistence::IScenePersistence& ScenePersistence;
		Ludus::Engine::Persistence::IRuntimeManifestPersistence& RuntimeManifestPersistence;
		Ludus::Engine::Persistence::IRuntimeLaunchSettingsPersistence& RuntimeLaunchSettingsPersistence;
		Ludus::Editor::Persistence::IProjectManifestPersistence& ProjectManifestPersistence;
		Ludus::Editor::Panels::PanelRegistry& PanelRegistry;
	};
}
