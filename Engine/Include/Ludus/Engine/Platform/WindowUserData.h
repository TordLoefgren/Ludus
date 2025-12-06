#pragma once

#include <Ludus/Engine/Events/EventBus.h>
#include <Ludus/Engine/Platform/Input.h>
#include <Ludus/Engine/Platform/WindowOptions.h>

namespace Ludus::Engine::Platform
{
	struct WindowUserData
	{
		Ludus::Engine::Events::EventBus* EventBus;
		WindowOptions Options;
	};
}