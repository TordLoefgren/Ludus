#pragma once

#include <Ludus/Editor/Persistence/IEditorPreferencesPersistence.h>
#include <Ludus/Editor/Persistence/IProjectManifestPersistence.h>
#include <Ludus/Engine/Persistence/IRuntimeLaunchSettingsPersistence.h>
#include <Ludus/Engine/Persistence/IRuntimeManifestPersistence.h>
#include <Ludus/Engine/Persistence/IScenePersistence.h>

namespace Ludus::Editor::Core
{
	struct EditorPersistenceContext
	{
		const Ludus::Editor::Persistence::IEditorPreferencesPersistence& EditorPreferences;
		const Ludus::Editor::Persistence::IProjectManifestPersistence& ProjectManifest;
		const Ludus::Engine::Persistence::IRuntimeLaunchSettingsPersistence& RuntimeLaunchSettings;
		const Ludus::Engine::Persistence::IRuntimeManifestPersistence& RuntimeManifest;
		const Ludus::Engine::Persistence::IScenePersistence& Scene;

		static EditorPersistenceContext Create(
			const Ludus::Editor::Persistence::IEditorPreferencesPersistence& editorPreferences,
			const Ludus::Editor::Persistence::IProjectManifestPersistence& projectManifest,
			const Ludus::Engine::Persistence::IRuntimeLaunchSettingsPersistence& runtimeLaunchSettings,
			const Ludus::Engine::Persistence::IRuntimeManifestPersistence& runtimeManifest,
			const Ludus::Engine::Persistence::IScenePersistence& scene
		)
		{
			return {
				.EditorPreferences = editorPreferences,
				.ProjectManifest = projectManifest,
				.RuntimeLaunchSettings = runtimeLaunchSettings,
				.RuntimeManifest = runtimeManifest,
				.Scene = scene
			};
		}
	};
}
