#pragma once

#include <filesystem>

#include <Ludus/Editor/Core/ProjectManifest.h>
#include <Ludus/Editor/Persistence/IProjectManifestPersistence.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Persistence/IRuntimeLaunchSettingsPersistence.h>
#include <Ludus/Engine/Persistence/IRuntimeManifestPersistence.h>
#include <Ludus/Engine/Persistence/IScenePersistence.h>
#include <Ludus/Engine/Runtime/RuntimeLaunchSettings.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::Editor::Persistence
{
	struct LoadedProjectData
	{
		Ludus::Editor::Core::ProjectManifest ProjectManifest;
		Ludus::Engine::Runtime::RuntimeManifest RuntimeManifest;
		Ludus::Engine::Runtime::RuntimeLaunchSettings RuntimeLaunchSettings;
		Ludus::Engine::Core::Scene EntryScene;
	};

	class ProjectSessionLoader
	{
	private:
		const Ludus::Engine::Persistence::IScenePersistence& m_ScenePersistence;
		const Ludus::Engine::Persistence::IRuntimeManifestPersistence& m_RuntimeManifestPersistence;
		const Ludus::Engine::Persistence::IRuntimeLaunchSettingsPersistence& m_RuntimeLaunchSettingsPersistence;
		const Ludus::Editor::Persistence::IProjectManifestPersistence& m_ProjectManifestPersistence;

	public:
		ProjectSessionLoader(
			const Ludus::Engine::Persistence::IScenePersistence& scenePersistence,
			const Ludus::Engine::Persistence::IRuntimeManifestPersistence& runtimeManifestPersistence,
			const Ludus::Engine::Persistence::IRuntimeLaunchSettingsPersistence& runtimeLaunchSettingsPersistence,
			const Ludus::Editor::Persistence::IProjectManifestPersistence& projectManifestPersistence
		);

		LoadedProjectData Load(const std::filesystem::path& projectManifestPath) const;
		LoadedProjectData Load(Ludus::Editor::Core::ProjectManifest projectManifest) const;
	};
}
