#pragma once

#include <Ludus/Scripting/ABI/Types.h>

namespace Ludus::Scripting::ABI
{
	struct ScriptAPI
	{
		std::uint32_t Size = sizeof(ScriptAPI);
		Version Version { };

		GetEntityByNameFn GetEntityByName = nullptr;
		LoadSceneByNameFn LoadSceneByName = nullptr;

		DebugFn Debug = nullptr;
		PrintFn Print = nullptr;

		GetKeyFn GetKey = nullptr;
		GetKeyDownFn GetKeyDown = nullptr;
		GetKeyUpFn GetKeyUp = nullptr;

		GetMouseButtonFn GetMouseButton = nullptr;
		GetMouseButtonDownFn GetMouseButtonDown = nullptr;
		GetMouseButtonUpFn GetMouseButtonUp = nullptr;

		GetDisplayNameFn GetDisplayName = nullptr;
		SetDisplayNameFn SetDisplayName = nullptr;

		GetRigidBodyFn GetRigidBody = nullptr;
		SetRigidBodyFn SetRigidBody = nullptr;

		GetTextFn GetText = nullptr;
		SetTextFn SetText = nullptr;

		GetTransformFn GetTransform = nullptr;
		SetTransformFn SetTransform = nullptr;
	};

	ScriptContext* CreateScriptContext(const ScriptAPI* api, void* host);
	void DestroyScriptContext(ScriptContext* context);

	const ScriptAPI* GetScriptAPI(const ScriptContext* context);
	void* GetScriptHost(const ScriptContext* context);
}
