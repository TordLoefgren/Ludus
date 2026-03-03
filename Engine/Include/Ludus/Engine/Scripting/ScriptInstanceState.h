#pragma once

#include <cstdint>

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Scripting/API/Types.h>

namespace Ludus::Engine::Scripting
{
	struct ScriptInstanceState
	{
		Ludus::Engine::Core::SceneHandle SceneHandle;
		Ludus::Engine::Core::EntityHandle OwnerHandle;
		const Ludus::Engine::Scripting::API::ScriptDefinition* Definition = nullptr;
		bool HasCreated = false;
		std::uint64_t LastSeenFrame = 0;
	};
}
