#pragma once

#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Engine::Runtime
{
	struct ScenePresentation
	{
		Ludus::Engine::Core::SceneId CurrentSceneId = Ludus::Engine::Core::SceneId::Invalid();
	};
}
