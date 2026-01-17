#pragma once

#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Core/Version.h>
#include <vector>

namespace Ludus::Engine::Core
{
	struct Project
	{
		Ludus::Engine::Core::Version Version;
		std::vector<Ludus::Engine::Core::Scene> Scenes;
	};
}
