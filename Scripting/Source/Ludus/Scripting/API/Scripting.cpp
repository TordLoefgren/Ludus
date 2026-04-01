#include "pch.h"

#include <stdexcept>

#include <Ludus/Scripting/ABI/ScriptContext.h>
#include <Ludus/Scripting/API/Scripting.h>

namespace Ludus::Scripting::API
{
	namespace
	{
		const ABI::ScriptAPI* ResolveAPI(ScriptContext* context)
		{
			const auto* api = ABI::GetScriptAPI(context);
			if (!api)
			{
				throw std::runtime_error("Script API was null.");
			}

			if (api->Size != sizeof(ABI::ScriptAPI))
			{
				throw std::runtime_error("Script API size mismatch.");
			}

			if (api->Version.Major != ABI::CurrentAPIVersion.Major ||
				api->Version.Minor != ABI::CurrentAPIVersion.Minor ||
				api->Version.Patch != ABI::CurrentAPIVersion.Patch)
			{
				throw std::runtime_error("Script API version mismatch.");
			}

			return api;
		}
	}

	bool GetEntityByName(ScriptContext* context, const char* name, EntityHandle* entityHandle)
	{
		const auto* api = ResolveAPI(context);
		return api && api->GetEntityByName ? api->GetEntityByName(context, name, entityHandle) : false;
	}

	void Debug(ScriptContext* context, const char* message)
	{
		const auto* api = ResolveAPI(context);
		if (!api || !api->Debug)
		{
			return;
		}

		api->Debug(context, message);
	}

	void Print(ScriptContext* context, const char* message)
	{
		const auto* api = ResolveAPI(context);
		if (!api || !api->Print)
		{
			return;
		}

		api->Print(context, message);
	}

	bool GetKey(ScriptContext* context, Key key)
	{
		const auto* api = ResolveAPI(context);
		return api && api->GetKey ? api->GetKey(context, key) : false;
	}

	bool GetKeyDown(ScriptContext* context, Key key)
	{
		const auto* api = ResolveAPI(context);
		return api && api->GetKeyDown ? api->GetKeyDown(context, key) : false;
	}

	bool GetKeyUp(ScriptContext* context, Key key)
	{
		const auto* api = ResolveAPI(context);
		return api && api->GetKeyUp ? api->GetKeyUp(context, key) : false;
	}

	bool GetMouseButton(ScriptContext* context, MouseButton mouseButton)
	{
		const auto* api = ResolveAPI(context);
		return api && api->GetMouseButton ? api->GetMouseButton(context, mouseButton) : false;
	}

	bool GetMouseButtonDown(ScriptContext* context, MouseButton mouseButton)
	{
		const auto* api = ResolveAPI(context);
		return api && api->GetMouseButtonDown ? api->GetMouseButtonDown(context, mouseButton) : false;
	}

	bool GetMouseButtonUp(ScriptContext* context, MouseButton mouseButton)
	{
		const auto* api = ResolveAPI(context);
		return api && api->GetMouseButtonUp ? api->GetMouseButtonUp(context, mouseButton) : false;
	}

	bool GetDisplayName(ScriptContext* context, EntityHandle entityHandle, DisplayNameData* displayName)
	{
		const auto* api = ResolveAPI(context);
		return api && api->GetDisplayName ? api->GetDisplayName(context, entityHandle, displayName) : false;
	}

	bool SetDisplayName(ScriptContext* context, EntityHandle entityHandle, const DisplayNameData* displayName)
	{
		const auto* api = ResolveAPI(context);
		return api && api->SetDisplayName ? api->SetDisplayName(context, entityHandle, displayName) : false;
	}

	bool GetRigidBody(ScriptContext* context, EntityHandle entityHandle, RigidBody2DData* rigidBody)
	{
		const auto* api = ResolveAPI(context);
		return api && api->GetRigidBody ? api->GetRigidBody(context, entityHandle, rigidBody) : false;
	}

	bool SetRigidBody(ScriptContext* context, EntityHandle entityHandle, const RigidBody2DData* rigidBody)
	{
		const auto* api = ResolveAPI(context);
		return api && api->SetRigidBody ? api->SetRigidBody(context, entityHandle, rigidBody) : false;
	}

	bool GetText(ScriptContext* context, EntityHandle entityHandle, Text2DData* text)
	{
		const auto* api = ResolveAPI(context);
		return api && api->GetText ? api->GetText(context, entityHandle, text) : false;
	}

	bool SetText(ScriptContext* context, EntityHandle entityHandle, const Text2DData* text)
	{
		const auto* api = ResolveAPI(context);
		return api && api->SetText ? api->SetText(context, entityHandle, text) : false;
	}

	bool GetTransform(ScriptContext* context, EntityHandle entityHandle, Transform2DData* transform)
	{
		const auto* api = ResolveAPI(context);
		return api && api->GetTransform ? api->GetTransform(context, entityHandle, transform) : false;
	}

	bool SetTransform(ScriptContext* context, EntityHandle entityHandle, const Transform2DData* transform)
	{
		const auto* api = ResolveAPI(context);
		return api && api->SetTransform ? api->SetTransform(context, entityHandle, transform) : false;
	}
}
