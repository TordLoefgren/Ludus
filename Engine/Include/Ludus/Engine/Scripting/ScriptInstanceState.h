#pragma once

#include <cstdint>
#include <unordered_map>

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Scripting/LoadedScriptDefinition.h>
#include <Ludus/Scripting/ABI/Types.h>

namespace Ludus::Engine::Scripting
{
	struct ScriptInstanceState
	{
		Ludus::Engine::Core::SceneId SceneId;
		Ludus::Engine::Core::EntityId OwnerId;
		const LoadedScriptDefinition* Definition = nullptr;
		bool HasCreated = false;
		std::unordered_map<Ludus::Engine::Core::EntityId, Ludus::Scripting::ABI::Collision2DData> ActiveCollisions;
		std::uint64_t LastSeenFrame = 0;
	};
}
