#pragma once

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Scripting/ScriptBindings.h>
#include <Ludus/Scripting/ABI/ScriptContext.h>
#include <Ludus/Scripting/ABI/Types.h>

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

namespace Ludus::Engine::Scripting
{
	class ScriptEngine
	{
	private:
		ScriptBindingsState* m_BindingsState = nullptr;
		Ludus::Scripting::ABI::ScriptContext* m_ScriptContext = nullptr;

		void CreateContext();
		void DestroyContext();

	public:
		ScriptEngine(
			const Ludus::Engine::Runtime::RuntimeManifest& runtimeManifest,
			Ludus::Engine::Core::SceneRegistry& sceneRegistry,
			Ludus::Engine::Runtime::SceneRuntimeState& sceneRuntimeState,
			Ludus::Engine::Windowing::Input& input
		);

		~ScriptEngine();

		Ludus::Scripting::ABI::ScriptContext* GetContext();

		void SetContextScene(Ludus::Engine::Core::SceneId sceneId);
	};
}
