#pragma once

#include <filesystem>
#include <memory>

#include <Ludus/Engine/Core/Project.h>
#include <Ludus/Engine/Core/ProjectContext.h>
#include <Ludus/Engine/Core/Random.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Core/Version.h>
#include <Ludus/Engine/Persistence/IProjectPersistence.h>
#include <Ludus/Engine/Persistence/IScenePersistence.h>
#include <Ludus/Engine/Persistence/LmlProjectPersistence.h>
#include <Ludus/Engine/Persistence/LmlScenePersistence.h>
#include <Ludus/Engine/Persistence/Paths.h>

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
		ProjectRepository(
			std::unique_ptr<Ludus::Engine::Persistence::IProjectPersistence> projectPersistence = std::make_unique<Ludus::Engine::Persistence::LmlProjectPersistence>(),
			std::unique_ptr<Ludus::Engine::Persistence::IScenePersistence> scenePersistence = std::make_unique<Ludus::Engine::Persistence::LmlScenePersistence>()
		) :
			m_ProjectPersistence(std::move(projectPersistence)),
			m_ScenePersistence(std::move(scenePersistence))
		{ }

#pragma region Projects

		std::filesystem::path GetScenesDirectory(const Ludus::Engine::Persistence::ProjectContext& context)
		{
			return Ludus::Engine::Persistence::Paths::ScenesDirectory(context.ProjectRootDirectory);
		}

		std::filesystem::path GetDefaultScenePath(const Ludus::Engine::Persistence::ProjectContext& context, std::string_view sceneName)
		{
			return Ludus::Engine::Persistence::Paths::SceneFile(context.ProjectRootDirectory, sceneName);
		}

		ProjectContext CreateProject(std::string_view name, std::optional<std::filesystem::path> path = std::nullopt)
		{
			const auto projectsRoot = path.value_or(Ludus::Engine::Persistence::Paths::ProjectsRoot());
			std::filesystem::create_directories(projectsRoot);

			const auto projectRoot = projectsRoot / std::string(name);
			if (std::filesystem::exists(projectRoot))
			{
				throw std::runtime_error("Project name aleady exists for path: " + projectRoot.string());
			}

			Ludus::Engine::Persistence::Paths::EnsureProjectLayoutExists(projectRoot);

			// Save default scene.
			Ludus::Engine::Core::Scene scene(m_Random.NextId(), "Sample Scene");
			auto& ecs = scene.EntityComponentSystem;
			const auto handle = ecs.AddEntity();
			ecs.AttachDisplayName(handle, "Main Camera");
			ecs.AttachTransform(handle);
			ecs.AttachCamera(handle);

			const auto scenePath = Ludus::Engine::Persistence::Paths::SceneFile(projectRoot, scene.Name);
			m_ScenePersistence->Save(scene, scenePath);

			// Save project.
			Ludus::Engine::Core::ProjectSceneReference sceneReference { scene.Handle, scene.Name, scenePath };
			std::vector<Ludus::Engine::Core::ProjectSceneReference> references { sceneReference };
			Ludus::Engine::Core::Project project(m_Version, references, scene.Handle);

			const auto projectPath = projectRoot / (std::string(name) + ".lproj");
			m_ProjectPersistence->Save(project, projectPath);

			return {
				.Project = std::move(project),
				.ProjectPath = projectPath,
				.ProjectRootDirectory = projectRoot
			};
		}

		Ludus::Engine::Persistence::ProjectContext LoadProject(const std::filesystem::path& path)
		{
			auto project = m_ProjectPersistence->Load(path);
			return {
				.Project = std::move(project),
				.ProjectPath = path,
				.ProjectRootDirectory = path.parent_path()
			};
		}

		void SaveProject(Ludus::Engine::Persistence::ProjectContext& context)
		{
			m_ProjectPersistence->Save(context.Project, context.ProjectPath);
		}

#pragma endregion

#pragma region Scenes

		Ludus::Engine::Core::Scene LoadScene(const Ludus::Engine::Persistence::ProjectContext& context, Ludus::Engine::Core::SceneHandle sceneHandle)
		{
			std::filesystem::path path;
			for (const auto& scene : context.Project.Scenes)
			{
				if (scene.Handle == sceneHandle)
				{
					path = scene.Path;
				}
			}

			if (path.empty())
			{
				throw std::runtime_error("No scene path found.");
			}

			return m_ScenePersistence->Load(path);
		}

		Ludus::Engine::Core::Scene LoadScene(const std::filesystem::path& path)
		{
			return m_ScenePersistence->Load(path);
		}

		void SaveScene(const Ludus::Engine::Persistence::ProjectContext& context, const Ludus::Engine::Core::Scene& scene, Ludus::Engine::Core::SceneHandle sceneHandle)
		{
			std::filesystem::path path;
			for (const auto& scene : context.Project.Scenes)
			{
				if (scene.Handle == sceneHandle)
				{
					path = scene.Path;
				}
			}

			if (path.empty())
			{
				throw std::runtime_error("No scene path found.");
			}

			m_ScenePersistence->Save(scene, path);
		}

		void SaveScene(const Ludus::Engine::Core::Scene& scene, const std::filesystem::path& path)
		{
			m_ScenePersistence->Save(scene, path);
		}

		void SaveSceneAs(const std::filesystem::path& path, Ludus::Engine::Persistence::ProjectContext& context, const Ludus::Engine::Core::Scene& scene, Ludus::Engine::Core::SceneHandle sceneHandle)
		{
			m_ScenePersistence->Save(scene, path);

			bool isUpdated = false;
			for (auto& sceneReference : context.Project.Scenes)
			{
				if (sceneReference.Handle == sceneHandle)
				{
					sceneReference.Path = path;

					isUpdated = true;
					break;
				}
			}

			if (!isUpdated)
			{
				throw std::runtime_error("Scene handle not found in project.");
			}

			SaveProject(context);
		}

#pragma endregion
	};
}
