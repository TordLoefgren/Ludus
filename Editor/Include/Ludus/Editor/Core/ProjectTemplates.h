#pragma once

#include <string_view>

#include <Ludus/Engine/Core/Random.h>
#include <Ludus/Engine/Core/Scene.h>

namespace Ludus::Editor::Core::ProjectTemplates
{
	inline Ludus::Engine::Core::Scene CreateDefaultScene(std::string_view name = "Sample Scene")
	{
		auto random = Ludus::Engine::Core::Random();
		Ludus::Engine::Core::Scene scene(
			{ random.NextId() },
			name
		);

		const auto entityId = scene.EntityComponentSystem.AddEntity();
		scene.EntityComponentSystem.AttachDisplayName(entityId, "Main Camera");
		scene.EntityComponentSystem.AttachTransform(entityId);
		scene.EntityComponentSystem.AttachCamera(entityId);

		return scene;
	}
}
