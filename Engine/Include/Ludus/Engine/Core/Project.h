#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Core/Version.h>

namespace Ludus::Engine::Core
{
	struct ProjectSceneReference
	{
		Ludus::Engine::Core::SceneHandle Handle = 0;
		std::string Name;
		std::filesystem::path Path;
	};

	struct Project
	{
		Ludus::Engine::Core::Version Version = { 0, 1, 0 };
		std::vector<ProjectSceneReference> Scenes;
		Ludus::Engine::Core::SceneHandle ActiveSceneHandle = 0;
	};
}
