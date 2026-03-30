#pragma once

#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Scripting/ABI/ScriptContext.h>
#include <Ludus/Scripting/ABI/Types.h>

namespace Ludus::Engine::Core
{
	struct SceneRegistry;
}

namespace Ludus::Engine::Windowing
{
	class Input;
}

namespace Ludus::Engine::Scripting
{
	struct ScriptBindingsState;

	ScriptBindingsState* CreateScriptBindingsState(
		Ludus::Engine::Core::SceneRegistry& sceneRegistry,
		Ludus::Engine::Windowing::Input& input,
		Ludus::Engine::Core::SceneHandle activeSceneHandle
	);

	void DestroyScriptBindingsState(ScriptBindingsState* state);

	void SetActiveScene(
		ScriptBindingsState* state,
		Ludus::Engine::Core::SceneHandle sceneHandle
	);

	const Ludus::Scripting::ABI::ScriptAPI* GetScriptAPI(
		const ScriptBindingsState* state
	);

	void* GetScriptHost(ScriptBindingsState* state);
}
