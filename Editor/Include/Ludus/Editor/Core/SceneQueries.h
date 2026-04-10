#pragma once

#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::Editor::Core::SceneQueries
{
	inline bool ContainsUnresolvedScriptReferences(
		const Ludus::Engine::Core::Scene& scene,
		const Ludus::Engine::Runtime::RuntimeManifest& runtimeManifest
	)
	{
		for (const auto& script : scene.EntityComponentSystem.Scripts.View())
		{
			if (!runtimeManifest.TryGetScriptReference(script.Id))
			{
				return true;
			}
		}

		return false;
	}
}
