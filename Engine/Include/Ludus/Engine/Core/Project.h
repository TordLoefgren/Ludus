#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <Ludus/Engine/Components/ScriptComponent.h>
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

	struct ProjectScriptReference
	{
		Ludus::Engine::Components::ScriptHandle Handle = 0;
		std::string Name;
	};

	struct Project
	{
		Ludus::Engine::Core::Version Version = { 0, 2, 0 };
		std::vector<ProjectSceneReference> Scenes;
		std::vector<ProjectScriptReference> Scripts;
		Ludus::Engine::Core::SceneHandle ActiveSceneHandle = 0;
	};
}
