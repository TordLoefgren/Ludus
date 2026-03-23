#pragma once

#include <filesystem>

#include <Ludus/Editor/Core/ProjectManifest.h>
#include <Ludus/Editor/Persistence/IProjectManifestPersistence.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Persistence/IRuntimeManifestPersistence.h>
#include <Ludus/Engine/Persistence/IScenePersistence.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::Editor::Persistence
{
	struct LoadedProjectData
	{
		Ludus::Editor::Core::ProjectManifest ProjectManifest;
		Ludus::Engine::Runtime::RuntimeManifest RuntimeManifest;
		Ludus::Engine::Core::Scene EntryScene;
	};

	class ProjectSessionLoader
	{
	private:
		Ludus::Engine::Persistence::IScenePersistence& m_ScenePersistence;
		Ludus::Engine::Persistence::IRuntimeManifestPersistence& m_RuntimeManifestPersistence;
		Ludus::Editor::Persistence::IProjectManifestPersistence& m_ProjectManifestPersistence;

	public:
		ProjectSessionLoader(
			Ludus::Engine::Persistence::IScenePersistence& scenePersistence,
			Ludus::Engine::Persistence::IRuntimeManifestPersistence& runtimeManifestPersistence,
			Ludus::Editor::Persistence::IProjectManifestPersistence& projectManifestPersistence
		);

		LoadedProjectData Load(const std::filesystem::path& projectManifestPath);
		LoadedProjectData Load(Ludus::Editor::Core::ProjectManifest projectManifest);
	};
}
