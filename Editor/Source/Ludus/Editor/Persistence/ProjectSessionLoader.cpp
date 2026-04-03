#include "pch.h"

#include <optional>
#include <stdexcept>
#include <utility>

#include <Ludus/Editor/Core/ProjectManifest.h>
#include <Ludus/Editor/Persistence/IProjectManifestPersistence.h>
#include <Ludus/Editor/Persistence/ProjectSessionLoader.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Persistence/IRuntimeManifestPersistence.h>
#include <Ludus/Engine/Persistence/IScenePersistence.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::Editor::Persistence
{
	namespace
	{
		std::optional<std::filesystem::path> TryFindScenePath(
			const Ludus::Engine::Runtime::RuntimeManifest& runtimeManifest,
			Ludus::Engine::Core::SceneHandle sceneHandle
		)
		{
			for (const auto& sceneReference : runtimeManifest.Scenes)
			{
				if (sceneReference.Handle == sceneHandle)
				{
					return sceneReference.Path;
				}
			}

			return std::nullopt;
		}
	}

	ProjectSessionLoader::ProjectSessionLoader(
		Ludus::Engine::Persistence::IScenePersistence& scenePersistence,
		Ludus::Engine::Persistence::IRuntimeManifestPersistence& runtimeManifestPersistence,
		Ludus::Editor::Persistence::IProjectManifestPersistence& projectManifestPersistence
	) :
		m_ScenePersistence(scenePersistence),
		m_RuntimeManifestPersistence(runtimeManifestPersistence),
		m_ProjectManifestPersistence(projectManifestPersistence)
	{ }

	LoadedProjectData ProjectSessionLoader::Load(const std::filesystem::path& projectManifestPath)
	{
		return Load(m_ProjectManifestPersistence.Load(projectManifestPath));
	}

	LoadedProjectData ProjectSessionLoader::Load(Ludus::Editor::Core::ProjectManifest projectManifest)
	{
		auto runtimeManifest = m_RuntimeManifestPersistence.Load(projectManifest.RuntimeManifestPath);
		const auto scenePath = TryFindScenePath(runtimeManifest, runtimeManifest.EntrySceneHandle);
		if (!scenePath)
		{
			throw std::runtime_error("No scene path in runtime manifest.");
		}

		auto scene = m_ScenePersistence.Load(*scenePath);

		return {
			.ProjectManifest = std::move(projectManifest),
			.RuntimeManifest = std::move(runtimeManifest),
			.EntryScene = std::move(scene)
		};
	}
}
