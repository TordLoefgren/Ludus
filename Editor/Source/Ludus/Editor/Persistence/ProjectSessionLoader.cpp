#include "pch.h"

#include <stdexcept>
#include <utility>

#include <Ludus/Editor/Core/ProjectManifest.h>
#include <Ludus/Editor/Persistence/IProjectManifestPersistence.h>
#include <Ludus/Editor/Persistence/ProjectSessionLoader.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Persistence/IRuntimeLaunchSettingsPersistence.h>
#include <Ludus/Engine/Persistence/IRuntimeManifestPersistence.h>
#include <Ludus/Engine/Persistence/IScenePersistence.h>
#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::Editor::Persistence
{
	ProjectSessionLoader::ProjectSessionLoader(
		Ludus::Engine::Persistence::IScenePersistence& scenePersistence,
		Ludus::Engine::Persistence::IRuntimeManifestPersistence& runtimeManifestPersistence,
		Ludus::Engine::Persistence::IRuntimeLaunchSettingsPersistence& runtimeLaunchSettingsPersistence,
		Ludus::Editor::Persistence::IProjectManifestPersistence& projectManifestPersistence
	) :
		m_ScenePersistence(scenePersistence),
		m_RuntimeManifestPersistence(runtimeManifestPersistence),
		m_RuntimeLaunchSettingsPersistence(runtimeLaunchSettingsPersistence),
		m_ProjectManifestPersistence(projectManifestPersistence)
	{ }

	LoadedProjectData ProjectSessionLoader::Load(const std::filesystem::path& projectManifestPath)
	{
		return Load(m_ProjectManifestPersistence.Load(projectManifestPath));
	}

	LoadedProjectData ProjectSessionLoader::Load(Ludus::Editor::Core::ProjectManifest projectManifest)
	{
		const auto runtimeName = projectManifest.ProjectRoot.filename().string();
		auto runtimeManifest = m_RuntimeManifestPersistence.Load(projectManifest.RuntimeManifestPath);
		auto runtimeLaunchSettings = m_RuntimeLaunchSettingsPersistence.Load(
			Ludus::Engine::Persistence::Paths::RuntimeLaunchSettingsFile(projectManifest.ProjectRoot, runtimeName)
		);

		const Ludus::Engine::Runtime::SceneReference* entrySceneReference = nullptr;
		if (runtimeManifest.EntrySceneId.IsValid())
		{
			for (const auto& sceneReference : runtimeManifest.Scenes)
			{
				if (sceneReference.Id == runtimeManifest.EntrySceneId)
				{
					entrySceneReference = &sceneReference;
					break;
				}
			}
		}

		if (!entrySceneReference)
		{
			throw std::runtime_error("Runtime manifest entry scene was not found.");
		}

		auto scene = m_ScenePersistence.Load(entrySceneReference->Path);

		for (const auto& script : scene.EntityComponentSystem.Scripts.View())
		{
			if (!runtimeManifest.TryGetScriptReference(script.Id))
			{
				LUDUS_LOG_WARN("Scene contains script id that is not present in the runtime manifest.");
			}
		}

		return {
			.ProjectManifest = std::move(projectManifest),
			.RuntimeManifest = std::move(runtimeManifest),
			.RuntimeLaunchSettings = std::move(runtimeLaunchSettings),
			.EntryScene = std::move(scene)
		};
	}
}
