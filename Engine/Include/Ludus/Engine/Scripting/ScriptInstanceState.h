#pragma once

#include <cstdint>
#include <unordered_map>

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Scripting/LoadedScriptDefinition.h>
#include <Ludus/Scripting/ABI/Types.h>

namespace Ludus::Engine::Scripting
{
	struct ScriptInstanceState
	{
		Ludus::Engine::Core::SceneHandle SceneHandle;
		Ludus::Engine::Core::EntityHandle OwnerHandle;
		const LoadedScriptDefinition* Definition = nullptr;
		bool HasCreated = false;
		std::unordered_map<Ludus::Engine::Core::EntityHandle, Ludus::Scripting::ABI::Collision2DData> ActiveCollisions;
		std::uint64_t LastSeenFrame = 0;
	};
}
