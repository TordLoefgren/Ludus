#pragma once

#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::Editor::Core::SceneQueries
{
	inline bool ContainsUnresolvedAssetReferences(
		const Ludus::Engine::Core::Scene& scene,
		const Ludus::Engine::Runtime::RuntimeManifest& runtimeManifest
	)
	{
		for (const auto& sprite : scene.EntityComponentSystem.Sprites.View())
		{
			if (!sprite.TextureId.IsValid())
			{
				continue;
			}

			if (!runtimeManifest.TryGetAssetReference(sprite.TextureId))
			{
				return true;
			}
		}

		return false;
	}

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
