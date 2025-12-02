#pragma once

#include <Ludus/Events/Event.h>

namespace Ludus::Events
{
	struct Eventhandler
	{
		virtual ~Eventhandler() = default;
		virtual bool ProcessEvent(const Event& event) = 0;
	};
}
