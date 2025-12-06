#pragma once

#include <Ludus/Engine/Events/Event.h>

namespace Ludus::Engine::Events
{
	struct Eventhandler
	{
		virtual ~Eventhandler() = default;
		virtual bool ProcessEvent(const Event& event) = 0;
	};
}
