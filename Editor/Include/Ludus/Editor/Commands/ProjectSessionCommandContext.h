#pragma once

#include <Ludus/Editor/Core/EditorShell.h>
#include <Ludus/Editor/Core/ProjectSession.h>
#include <Ludus/Editor/Panels/PanelRegistry.h>
#include <Ludus/Editor/Persistence/IProjectManifestPersistence.h>
#include <Ludus/Engine/Persistence/IRuntimeManifestPersistence.h>
#include <Ludus/Engine/Persistence/IScenePersistence.h>
#include <Ludus/Engine/Runtime/IHostContext.h>

namespace Ludus::Editor::Commands
{
	struct ProjectSessionCommandContext
	{
		Ludus::Editor::Core::EditorShell& Shell;
		Ludus::Editor::Core::ProjectSession& ProjectSession;
		Ludus::Engine::Runtime::IHostContext& HostContext;
		Ludus::Engine::Persistence::IScenePersistence& ScenePersistence;
		Ludus::Engine::Persistence::IRuntimeManifestPersistence& RuntimeManifestPersistence;
		Ludus::Editor::Persistence::IProjectManifestPersistence& ProjectManifestPersistence;
		Ludus::Editor::Panels::PanelRegistry& PanelRegistry;
	};
}
