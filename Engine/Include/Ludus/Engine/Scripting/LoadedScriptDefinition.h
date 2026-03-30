#pragma once

#include <string>

#include <Ludus/Scripting/ABI/Types.h>

namespace Ludus::Engine::Scripting
{
	struct LoadedScriptDefinition
	{
		std::string Name;
		Ludus::Scripting::ABI::OnCreateFn OnCreate = nullptr;
		Ludus::Scripting::ABI::OnDestroyFn OnDestroy = nullptr;
		Ludus::Scripting::ABI::OnUpdateFn OnUpdate = nullptr;
		Ludus::Scripting::ABI::OnCollisionEnterFn OnCollisionEnter = nullptr;
		Ludus::Scripting::ABI::OnCollisionStayFn OnCollisionStay = nullptr;
		Ludus::Scripting::ABI::OnCollisionExitFn OnCollisionExit = nullptr;
	};
}
