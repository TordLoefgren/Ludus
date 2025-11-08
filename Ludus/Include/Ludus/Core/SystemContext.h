#pragma once

#include <Ludus/Engine/EntityComponentSystem.h>
#include <Ludus/Events/EventBus.h>
#include <Ludus/Platform/Input.h>

namespace Ludus::Core
{
	struct SystemContext
	{
		Ludus::Engine::EntityComponentSystem& EntityComponentSystem;
		Ludus::Events::EventBus& EventBus;
		Ludus::Platform::Input& Input;
	};
}
