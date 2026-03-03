#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string_view>

#include <Ludus/Engine/Core/Project.h>
#include <Ludus/Engine/Core/ProjectContext.h>
#include <Ludus/Engine/Core/Random.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Core/Version.h>
#include <Ludus/Engine/Persistence/IProjectPersistence.h>
#include <Ludus/Engine/Persistence/IScenePersistence.h>

namespace Ludus::Engine::Persistence
{
	struct ProjectRepository
	{
	private:
		const Ludus::Engine::Core::Version m_Version = { 0, 2, 0 };
		Ludus::Engine::Core::Random m_Random;

		std::unique_ptr<Ludus::Engine::Persistence::IProjectPersistence> m_ProjectPersistence;
		std::unique_ptr<Ludus::Engine::Persistence::IScenePersistence> m_ScenePersistence;

	public:
		ProjectRepository();
		ProjectRepository(
			std::unique_ptr<Ludus::Engine::Persistence::IProjectPersistence> projectPersistence,
			std::unique_ptr<Ludus::Engine::Persistence::IScenePersistence> scenePersistence
		);

		std::filesystem::path GetScenesDirectory(const Ludus::Engine::Core::ProjectContext& context);
		std::filesystem::path GetDefaultScenePath(const Ludus::Engine::Core::ProjectContext& context, std::string_view sceneName);

		Ludus::Engine::Core::ProjectContext CreateProject(std::string_view name, std::optional<std::filesystem::path> path = std::nullopt);
		Ludus::Engine::Core::ProjectContext LoadProject(const std::filesystem::path& path);
		void SaveProject(Ludus::Engine::Core::ProjectContext& context);

		Ludus::Engine::Core::Scene LoadScene(const Ludus::Engine::Core::ProjectContext& context, Ludus::Engine::Core::SceneHandle sceneHandle);
		Ludus::Engine::Core::Scene LoadScene(const std::filesystem::path& path);
		void SaveScene(const Ludus::Engine::Core::ProjectContext& context, const Ludus::Engine::Core::Scene& scene, Ludus::Engine::Core::SceneHandle sceneHandle);
		void SaveScene(const Ludus::Engine::Core::Scene& scene, const std::filesystem::path& path);
		void SaveSceneAs(const std::filesystem::path& path, Ludus::Engine::Core::ProjectContext& context, const Ludus::Engine::Core::Scene& scene, Ludus::Engine::Core::SceneHandle sceneHandle);
	};
}
