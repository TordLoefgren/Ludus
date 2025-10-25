#pragma once

#include <Ludus/Engine/EntityComponentSystem.h>
#include <Ludus/Platform/Input.h>

namespace Ludus::Core
{
	struct SystemContext
	{
		Ludus::Engine::EntityComponentSystem& EntityComponentSystem;
		Ludus::Platform::Input& Input;
	};
}
