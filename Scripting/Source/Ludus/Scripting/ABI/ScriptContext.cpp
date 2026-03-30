#include "pch.h"

#include <Ludus/Scripting/ABI/ScriptContext.h>

namespace Ludus::Scripting::ABI
{
	struct ScriptContext
	{
		const ScriptAPI* Api = nullptr;
		void* Host = nullptr;
	};

	namespace
	{
		const ScriptAPI* ResolveAPI(const ScriptContext* context)
		{
			if (!context)
			{
				return nullptr;
			}

			return context->Api;
		}
	}

	ScriptContext* CreateScriptContext(const ScriptAPI* api, void* host)
	{
		return new ScriptContext
		{
			.Api = api,
			.Host = host
		};
	}

	void DestroyScriptContext(ScriptContext* context)
	{
		delete context;
	}

	const ScriptAPI* GetScriptAPI(const ScriptContext* context)
	{
		return ResolveAPI(context);
	}

	void* GetScriptHost(const ScriptContext* context)
	{
		if (!context)
		{
			return nullptr;
		}

		return context->Host;
	}
}
