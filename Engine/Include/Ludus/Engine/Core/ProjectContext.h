#pragma once

#include <filesystem>
#include <optional>

#include <Ludus/Engine/Core/Project.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>

namespace Ludus::Engine::Persistence
{
	struct ProjectContext
	{
		Ludus::Engine::Core::Project Project;
		std::filesystem::path ProjectPath;
		std::filesystem::path ProjectRootDirectory;

		bool HasSceneReference(Ludus::Engine::Core::SceneHandle handle)
		{
			for (const auto& reference : Project.Scenes)
			{
				if (reference.Handle == handle)
				{
					return true;
				}
			}

			return false;
		}

		std::filesystem::path FindScenePath(Ludus::Engine::Core::SceneHandle handle)
		{
			for (const auto& reference : Project.Scenes)
			{
				if (reference.Handle == handle)
				{
					return reference.Path;
				}
			}

			throw std::runtime_error("Project did not contain the expected scene path.");
		}

		std::optional<Ludus::Engine::Core::SceneHandle> TryFindSceneHandleByPath(const std::filesystem::path& path)
		{
			for (const auto& reference : Project.Scenes)
			{
				if (Ludus::Engine::FileSystem::ArePathsEqual(reference.Path, path))
				{
					return reference.Handle;
				}
			}

			return std::nullopt;
		}

		void AddOrUpdateSceneReference(Ludus::Engine::Core::SceneHandle handle, std::string name, std::filesystem::path path)
		{
			for (auto& reference : Project.Scenes)
			{
				if (reference.Handle == handle)
				{
					reference.Name = std::move(name);
					reference.Path = std::move(path);

					return;
				}
			}

			Project.Scenes.push_back({ handle, std::move(name), std::move(path) });
		}
	};
}
