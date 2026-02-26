#include "pch.h"

#include <stdexcept>
#include <vector>

#include <Ludus/Engine/Persistence/LmlProjectPersistence.h>
#include <Ludus/Engine/Persistence/LmlScenePersistence.h>
#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/Engine/Persistence/ProjectRepository.h>

namespace Ludus::Engine::Persistence
{
	ProjectRepository::ProjectRepository() :
		m_ProjectPersistence(std::make_unique<Ludus::Engine::Persistence::LmlProjectPersistence>()),
		m_ScenePersistence(std::make_unique<Ludus::Engine::Persistence::LmlScenePersistence>())
	{ }

	ProjectRepository::ProjectRepository(
		std::unique_ptr<Ludus::Engine::Persistence::IProjectPersistence> projectPersistence,
		std::unique_ptr<Ludus::Engine::Persistence::IScenePersistence> scenePersistence
	) :
		m_ProjectPersistence(std::move(projectPersistence)),
		m_ScenePersistence(std::move(scenePersistence))
	{ }

	std::filesystem::path ProjectRepository::GetScenesDirectory(const Ludus::Engine::Persistence::ProjectContext& context)
	{
		return Ludus::Engine::Persistence::Paths::ScenesDirectory(context.ProjectRootDirectory);
	}

	std::filesystem::path ProjectRepository::GetDefaultScenePath(
		const Ludus::Engine::Persistence::ProjectContext& context,
		const std::string_view sceneName
	)
	{
		return Ludus::Engine::Persistence::Paths::SceneFile(context.ProjectRootDirectory, sceneName);
	}

	ProjectContext ProjectRepository::CreateProject(std::string_view name, std::optional<std::filesystem::path> path)
	{
		const auto projectsRoot = path.value_or(Ludus::Engine::Persistence::Paths::ProjectsRoot());
		std::filesystem::create_directories(projectsRoot);

		const auto projectRoot = projectsRoot / std::string(name);
		if (std::filesystem::exists(projectRoot))
		{
			throw std::runtime_error("Project name aleady exists for path: " + projectRoot.string());
		}

		Ludus::Engine::Persistence::Paths::EnsureProjectLayoutExists(projectRoot);

		Ludus::Engine::Core::Scene scene(m_Random.NextId(), "Sample Scene");
		auto& ecs = scene.EntityComponentSystem;
		const auto handle = ecs.AddEntity();
		ecs.AttachDisplayName(handle, "Main Camera");
		ecs.AttachTransform(handle);
		ecs.AttachCamera(handle);

		const auto scenePath = Ludus::Engine::Persistence::Paths::SceneFile(projectRoot, scene.Name);
		m_ScenePersistence->Save(scene, scenePath);

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

	Ludus::Engine::Persistence::ProjectContext ProjectRepository::LoadProject(const std::filesystem::path& path)
	{
		auto project = m_ProjectPersistence->Load(path);
		return {
			.Project = std::move(project),
			.ProjectPath = path,
			.ProjectRootDirectory = path.parent_path()
		};
	}

	void ProjectRepository::SaveProject(Ludus::Engine::Persistence::ProjectContext& context)
	{
		m_ProjectPersistence->Save(context.Project, context.ProjectPath);
	}

	Ludus::Engine::Core::Scene ProjectRepository::LoadScene(
		const Ludus::Engine::Persistence::ProjectContext& context,
		Ludus::Engine::Core::SceneHandle sceneHandle
	)
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

	Ludus::Engine::Core::Scene ProjectRepository::LoadScene(const std::filesystem::path& path)
	{
		return m_ScenePersistence->Load(path);
	}

	void ProjectRepository::SaveScene(
		const Ludus::Engine::Persistence::ProjectContext& context,
		const Ludus::Engine::Core::Scene& scene,
		Ludus::Engine::Core::SceneHandle sceneHandle
	)
	{
		std::filesystem::path path;
		for (const auto& sceneReference : context.Project.Scenes)
		{
			if (sceneReference.Handle == sceneHandle)
			{
				path = sceneReference.Path;
			}
		}

		if (path.empty())
		{
			throw std::runtime_error("No scene path found.");
		}

		m_ScenePersistence->Save(scene, path);
	}

	void ProjectRepository::SaveScene(const Ludus::Engine::Core::Scene& scene, const std::filesystem::path& path)
	{
		m_ScenePersistence->Save(scene, path);
	}

	void ProjectRepository::SaveSceneAs(
		const std::filesystem::path& path,
		Ludus::Engine::Persistence::ProjectContext& context,
		const Ludus::Engine::Core::Scene& scene,
		Ludus::Engine::Core::SceneHandle sceneHandle
	)
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
}
