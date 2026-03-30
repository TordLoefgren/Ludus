#pragma once

#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::Editor::Build
{
	inline void RewriteScenePathsForPackagedRuntime(Ludus::Engine::Runtime::RuntimeManifest& runtimeManifest)
	{
		for (auto& scene : runtimeManifest.Scenes)
		{
			scene.Path = Ludus::Engine::Persistence::Paths::RuntimeRelativeSceneFile(scene.Path);
		}
	}
}
