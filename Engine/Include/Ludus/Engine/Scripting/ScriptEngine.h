#pragma once

#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Scripting/ScriptBindings.h>
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
	class ScriptEngine
	{
	private:
		ScriptBindingsState* m_BindingsState = nullptr;
		Ludus::Scripting::ABI::ScriptContext* m_ScriptContext = nullptr;

		void CreateContext();
		void DestroyContext();

	public:
		ScriptEngine(
			Ludus::Engine::Core::SceneRegistry& sceneRegistry,
			Ludus::Engine::Windowing::Input& input,
			Ludus::Engine::Core::SceneHandle activeSceneHandle
		);

		~ScriptEngine();

		Ludus::Scripting::ABI::ScriptContext* GetContext();

		void SetActiveScene(Ludus::Engine::Core::SceneHandle sceneHandle);
	};
}
