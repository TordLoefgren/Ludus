#pragma once

#include <filesystem>
#include <optional>
#include <string_view>
#include <vector>

#include <Ludus/Engine/Components/ScriptComponent.h>
#include <Ludus/Engine/Core/Project.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>

namespace Ludus::Engine::Core
{
	struct ProjectContext
	{
		Ludus::Engine::Core::Project Project;
		std::filesystem::path ProjectPath;
		std::filesystem::path ProjectRootDirectory;

#pragma region Scenes

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

		bool AddOrUpdateSceneReference(Ludus::Engine::Core::SceneHandle handle, std::string name, std::filesystem::path path)
		{
			for (auto& reference : Project.Scenes)
			{
				if (reference.Handle == handle)
				{
					const bool hasNameChange = reference.Name != name;
					const bool hasPathChange = reference.Path != path;
					if (!hasNameChange && !hasPathChange)
					{
						return false;
					}

					reference.Name = std::move(name);
					reference.Path = std::move(path);

					return true;
				}
			}

			Project.Scenes.push_back({ handle, std::move(name), std::move(path) });
			return true;
		}

#pragma endregion

#pragma region Scripts

		bool HasScriptReference(Ludus::Engine::Components::ScriptHandle handle)
		{
			for (const auto& reference : Project.Scripts)
			{
				if (reference.Handle == handle)
				{
					return true;
				}
			}

			return false;
		}

		bool AddOrUpdateScriptReference(Ludus::Engine::Components::ScriptHandle handle, std::string name)
		{
			for (auto& reference : Project.Scripts)
			{
				if (reference.Handle == handle)
				{
					if (reference.Name == name)
					{
						return false;
					}

					reference.Name = std::move(name);

					return true;
				}
			}

			Project.Scripts.push_back({ handle, std::move(name) });
			return true;
		}

		std::optional<Ludus::Engine::Components::ScriptHandle> TryFindScriptHandleByName(std::string_view name) const
		{
			for (const auto& reference : Project.Scripts)
			{
				if (reference.Name == name)
				{
					return reference.Handle;
				}
			}

			return std::nullopt;
		}

		std::vector<std::string> GetScriptNames() const
		{
			std::vector<std::string> names;
			names.reserve(Project.Scripts.size());

			for (const auto& reference : Project.Scripts)
			{
				names.push_back(reference.Name);
			}

			return names;
		}

#pragma endregion

	};
}
