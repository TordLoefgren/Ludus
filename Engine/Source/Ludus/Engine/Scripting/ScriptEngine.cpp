#include "pch.h"

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Scripting/ScriptBindings.h>
#include <Ludus/Engine/Scripting/ScriptEngine.h>
#include <Ludus/Engine/Windowing/Input.h>
#include <Ludus/Scripting/ABI/ScriptContext.h>
#include <Ludus/Scripting/ABI/Types.h>

namespace Ludus::Engine::Scripting
{
	void ScriptEngine::CreateContext()
	{
		m_ScriptContext = Ludus::Scripting::ABI::CreateScriptContext(
			GetScriptAPI(m_BindingsState),
			GetScriptHost(m_BindingsState)
		);
	}

	void ScriptEngine::DestroyContext()
	{
		Ludus::Scripting::ABI::DestroyScriptContext(m_ScriptContext);
		m_ScriptContext = nullptr;
	}

	ScriptEngine::ScriptEngine(
		Ludus::Engine::Core::SceneRegistry& sceneRegistry,
		Ludus::Engine::Windowing::Input& input,
		Ludus::Engine::Core::SceneId activeSceneId
	) : m_BindingsState(CreateScriptBindingsState(sceneRegistry, input, activeSceneId))
	{
		CreateContext();
	}

	ScriptEngine::~ScriptEngine()
	{
		DestroyContext();
		DestroyScriptBindingsState(m_BindingsState);
		m_BindingsState = nullptr;
	}

	Ludus::Scripting::ABI::ScriptContext* ScriptEngine::GetContext()
	{
		return m_ScriptContext;
	}

	void ScriptEngine::SetActiveScene(Ludus::Engine::Core::SceneId sceneId)
	{
		Ludus::Engine::Scripting::SetActiveScene(m_BindingsState, sceneId);
	}
}
