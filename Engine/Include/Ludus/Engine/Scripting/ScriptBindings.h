#pragma once

#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Engine::Core
{
	struct SceneRegistry;
}

namespace Ludus::Engine::Runtime
{
	struct RuntimeManifest;
	struct SceneRuntimeState;

}
namespace Ludus::Engine::Windowing
{
	class Input;
}

namespace Ludus::Scripting::ABI
{
	struct ScriptAPI;
}

namespace Ludus::Engine::Scripting
{
	struct ScriptBindingsState;

	ScriptBindingsState* CreateScriptBindingsState(
		const Ludus::Engine::Runtime::RuntimeManifest& runtimeManifest,
		Ludus::Engine::Core::SceneRegistry& sceneRegistry,
		Ludus::Engine::Runtime::SceneRuntimeState& sceneRuntimeState,
		Ludus::Engine::Windowing::Input& input
	);

	void DestroyScriptBindingsState(ScriptBindingsState* state);

	void SetContextScene(
		ScriptBindingsState* state,
		Ludus::Engine::Core::SceneId sceneId
	);

	const Ludus::Scripting::ABI::ScriptAPI* GetScriptAPI(
		const ScriptBindingsState* state
	);

	void* GetScriptHost(ScriptBindingsState* state);
}
