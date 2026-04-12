#pragma once

#include <Ludus/Engine/Runtime/PendingSceneTransition.h>
#include <Ludus/Engine/Runtime/ScenePresentation.h>

namespace Ludus::Engine::Runtime
{
	struct SceneRuntimeState
	{
		ScenePresentation Presentation;
		PendingSceneTransition PendingTransition = PendingSceneTransition::NoneState();
	};
}
