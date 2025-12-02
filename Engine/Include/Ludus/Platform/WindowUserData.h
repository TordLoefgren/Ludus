#pragma once

#include <Ludus/Events/EventBus.h>
#include <Ludus/Platform/Input.h>
#include <Ludus/Platform/WindowOptions.h>

namespace Ludus::Platform
{
	struct WindowUserData
	{
		Ludus::Events::EventBus* EventBus;
		WindowOptions Options;
	};
}