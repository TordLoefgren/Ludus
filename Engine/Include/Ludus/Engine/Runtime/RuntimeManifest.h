#pragma once

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

#include <Ludus/Engine/Components/ScriptComponent.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Core/Version.h>

namespace Ludus::Engine::Runtime
{
	struct SceneReference
	{
		Ludus::Engine::Core::SceneHandle Handle = 0;
		std::string Name;
		std::filesystem::path Path;
	};

	struct ScriptReference
	{
		Ludus::Engine::Components::ScriptHandle Handle = 0;
		std::string Name;
	};

	struct RuntimeManifest
	{
		inline static constexpr Ludus::Engine::Core::Version CurrentVersion = { 0, 2, 0 };

		Ludus::Engine::Core::Version Version = CurrentVersion;
		Ludus::Engine::Core::SceneHandle EntrySceneHandle = 0;
		std::vector<SceneReference> Scenes;
		std::vector<ScriptReference> Scripts;

		static RuntimeManifest Create(
			Ludus::Engine::Core::SceneHandle entrySceneHandle = 0,
			std::vector<SceneReference> scenes = { },
			std::vector<ScriptReference> scripts = { }
		)
		{
			return {
				.Version = CurrentVersion,
				.EntrySceneHandle = entrySceneHandle,
				.Scenes = std::move(scenes),
				.Scripts = std::move(scripts)
			};
		}
	};
}
