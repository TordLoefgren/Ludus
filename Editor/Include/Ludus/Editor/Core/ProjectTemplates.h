#pragma once

#include <Ludus/Engine/Core/Random.h>
#include <Ludus/Engine/Core/Scene.h>

namespace Ludus::Editor::Core::ProjectTemplates
{
	inline Ludus::Engine::Core::Scene CreateDefaultScene()
	{
		auto random = Ludus::Engine::Core::Random();
		Ludus::Engine::Core::Scene scene(
			{ random.NextId() },
			"Sample Scene"
		);

		const auto entityId = scene.EntityComponentSystem.AddEntity();
		scene.EntityComponentSystem.AttachDisplayName(entityId, "Main Camera");
		scene.EntityComponentSystem.AttachTransform(entityId);
		scene.EntityComponentSystem.AttachCamera(entityId);

		return scene;
	}
}
