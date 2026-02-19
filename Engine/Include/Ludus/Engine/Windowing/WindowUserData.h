#pragma once

#include <Ludus/Engine/Events/EventBus.h>

namespace Ludus::Engine::Windowing
{
	struct WindowUserData
	{
		Ludus::Engine::Events::EventBus* EventBus;
	};
}
