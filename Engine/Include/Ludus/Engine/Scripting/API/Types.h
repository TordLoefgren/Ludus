#pragma once

#include <cstdint>

namespace Ludus::Engine::Scripting::API
{
	using EntityId = uint64_t;

	struct ScriptContext
	{
		void (*Debug)(const char* message) = nullptr;
		void (*Print)(const char* message) = nullptr;
	};

	using OnCreateFn = void(*)(EntityId, ScriptContext*);
	using OnDestroyFn = void(*)(EntityId, ScriptContext*);
	using OnUpdateFn = void(*)(EntityId, float, ScriptContext*);

	struct ScriptDefinition
	{
		const char* Name = nullptr;

		OnCreateFn OnCreate = nullptr;
		OnDestroyFn OnDestroy = nullptr;
		OnUpdateFn OnUpdate = nullptr;
	};

	struct Registry;

	using RegisterScriptsFn = void(*)(Registry* registry);

	inline constexpr const char* RegisterSymbolName = "RegisterScripts";
}
